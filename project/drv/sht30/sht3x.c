/*
 * sht3x.c
 *
 *  Created on: Aug 23, 2025
 *      Author: Administrator
 */


#include "sht3x.h"

struct i2c_client *client_for_drv;

/**
 * Registers addresses.
 */
typedef enum
{
	SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06,
	SHT3X_COMMAND_CLEAR_STATUS = 0x3041,
	SHT3X_COMMAND_SOFT_RESET = 0x30A2,
	SHT3X_COMMAND_HEATER_ENABLE = 0x306d,
	SHT3X_COMMAND_HEATER_DISABLE = 0x3066,
	SHT3X_COMMAND_READ_STATUS = 0xf32d,
	SHT3X_COMMAND_FETCH_DATA = 0xe000,
	SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737,
	SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272a
} sht3x_command_t;

static uint8_t calculate_crc(const uint8_t *data, size_t length)
{
	uint8_t crc = 0xff;\
    size_t i,j;
	for (i = 0; i < length; i++) {
		crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if ((crc & 0x80u) != 0) {
				crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
			} else {
				crc <<= 1u;
			}
		}
	}
	return crc;
}

static bool sht3x_send_command(sht3x_command_t command)
{
	uint8_t command_buffer[2] = {(command & 0xff00u) >> 8u, command & 0xffu};

	if (i2c_master_send(client_for_drv, command_buffer, sizeof(command_buffer))) {
		return false;
	}

	return true;
}

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb)
{
	return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

bool sht3x_init(struct i2c_client *client)
{
    int ret;
    uint8_t calculated_crc;
    uint8_t status_reg_and_checksum[3];
    uint8_t cmd[2];

    client_for_drv = client;

    cmd[0] = (SHT3X_COMMAND_READ_STATUS >> 8) & 0xFF;  // MSB
    cmd[1] = SHT3X_COMMAND_READ_STATUS & 0xFF;         // LSB
    
    ret = i2c_master_send(client_for_drv, cmd, 2);
    if (ret < 0) {
        dev_err(&client_for_drv->dev, "Failed to send cmd reg\n");
        return false;
    }

    ret = i2c_master_recv(client, status_reg_and_checksum, 3);
    if (ret < 0){
        dev_err(&client_for_drv->dev, "Failed to recv reg\n");
        return ret;
    }

    calculated_crc = calculate_crc(status_reg_and_checksum, 2);

	if (calculated_crc != status_reg_and_checksum[2]) {
        dev_err(&client_for_drv->dev, "CRC mismatch: calc=0x%02x recv=0x%02x\n",
                calculated_crc, status_reg_and_checksum[2]);
		return false;
	}

	return true;
}

bool sht3x_read_temperature_and_humidity(int *temperature, int *humidity)
{
    uint8_t buffer[6];
    int ret;
    uint8_t temperature_crc, humidity_crc;
    uint16_t temperature_raw, humidity_raw;

	sht3x_send_command(SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH);
	msleep(1);

    ret = i2c_master_recv(client_for_drv, buffer, sizeof(buffer));
    if (ret < 0) {
        dev_err(&client_for_drv->dev, "Failed to receive I2C data\n");
        return false;
    }

	temperature_crc = calculate_crc(buffer, 2);
	humidity_crc = calculate_crc(buffer + 3, 2);
	if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
		return false;
	}

	temperature_raw = uint8_to_uint16(buffer[0], buffer[1]);
	humidity_raw = uint8_to_uint16(buffer[3], buffer[4]);

	*temperature = -45000 + (175000 * temperature_raw) / 65535;
	*humidity = (100000 * humidity_raw) / 65535;

	return true;
}

bool sht3x_set_header_enable(bool enable)
{
	if (enable) {
		return sht3x_send_command(SHT3X_COMMAND_HEATER_ENABLE);
	} else {
		return sht3x_send_command(SHT3X_COMMAND_HEATER_DISABLE);
	}
}
