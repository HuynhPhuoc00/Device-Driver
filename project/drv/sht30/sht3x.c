#include "sht3x.h"

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

static int i2c_master_send_impl(struct i2c_client *client,
								uint8_t *pTxbuffer,
								uint32_t len){
	return i2c_master_send(client, pTxbuffer, len);
}
static int i2c_master_read_impl(struct i2c_client *client,
								uint8_t *buffer,
								uint8_t size){
	return i2c_master_recv(client, buffer, size);
}

static void msdelay_impl(unsigned ms)  { msleep(ms); }

sht3x_bus_ops_t ops = {
	.i2c_master_send = i2c_master_send_impl,
	.i2c_master_read = i2c_master_read_impl,
	.ms_delay = msdelay_impl,
};


sht3x_handle_t sht;

void sht3x_init(struct i2c_client *client){
	int ret;
	sht.ops = ops;
	sht.device_address = SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW;
	sht.g_client = client;
	ret = sht3x_check_init();
	if(!ret){
//		pr_info
	}
}

static uint8_t calculate_crc(const uint8_t *data, size_t length){
	uint8_t crc = 0xff;
	size_t i, j;
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

bool sht3x_send_command(sht3x_command_t command){
	uint8_t command_buffer[2] = {(command & 0xff00u) >> 8u, command & 0xffu};
	sht.ops.i2c_master_send(sht.g_client,
							command_buffer,
							sizeof(command_buffer));
	return true;
}

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb){
	return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

bool sht3x_check_init(void){

//	assert(sht.i2c_handle->Init.NoStretchMode == I2C_NOSTRETCH_DISABLE);
	// TODO: Assert i2c frequency is not too high
	int ret;
	uint8_t cmd[2];
	uint8_t status_reg_and_checksum[3], calculated_crc;

	cmd[0] = (SHT3X_COMMAND_READ_STATUS >> 8) & 0xFF;  // MSB
    cmd[1] = SHT3X_COMMAND_READ_STATUS & 0xFF;         // LSB
    
	ret = sht.ops.i2c_master_read(sht.g_client, cmd, 2);
    if (ret < 0) {
        dev_err(&sht.g_client->dev, "Failed to send cmd reg\n");
        return false;
    }

    // Read 3 byte: [status_MSB][status_LSB][CRC]
    ret = sht.ops.i2c_master_read(sht.g_client, status_reg_and_checksum, 3);
    if (ret < 0) {
        dev_err(&sht.g_client->dev, "Failed to recv status\n");
        return false;
    }

	calculated_crc = calculate_crc(status_reg_and_checksum, 2);

//	if (calculated_crc != status_reg_and_checksum[2]) {
//		return false;
//	}

	return true;
}

bool sht3x_read_temperature_and_humidity(int *temperature, int *humidity){
	uint8_t buffer[6], temperature_crc, humidity_crc;
	uint16_t temperature_raw, humidity_raw;

	sht3x_send_command(SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH);

	sht.ops.ms_delay(1);

	sht.ops.i2c_master_read(sht.g_client,
							buffer,
							sizeof(buffer));

	temperature_crc = calculate_crc(buffer, 2);
	humidity_crc = calculate_crc(buffer + 3, 2);
	if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
		return false;
	}

	temperature_raw = uint8_to_uint16(buffer[0], buffer[1]);
	humidity_raw = uint8_to_uint16(buffer[3], buffer[4]);

	*temperature = -45 + 175 * temperature_raw / 65535;
	*humidity = 100 * humidity_raw / 65535;

	return true;
}

bool sht3x_set_header_enable(bool enable){
	if (enable) {
		return sht3x_send_command(SHT3X_COMMAND_HEATER_ENABLE);
	} else {
		return sht3x_send_command(SHT3X_COMMAND_HEATER_DISABLE);
	}
}
