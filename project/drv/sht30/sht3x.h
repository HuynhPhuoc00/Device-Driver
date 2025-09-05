#pragma once

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include<linux/kdev_t.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/types.h>

#ifndef SHT3X_I2C_TIMEOUT
#define SHT3X_I2C_TIMEOUT 30
#endif

#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW 0x44
#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x45

//I2C_Handle_t i2c_handle;

typedef struct{
	int (*i2c_master_send)(struct i2c_client *client,
								uint8_t *pTxbuffer,
								uint32_t len);
	int (*i2c_master_read)(struct i2c_client *client,
								uint8_t *buffer,
								uint8_t size);

	void (*ms_delay)(unsigned int ms);
}sht3x_bus_ops_t;

/**
 * Structure defining a handle describing a SHT3x device.
 */
typedef struct {

	/**
	 * The handle to the I2C bus for the device.
	 */
	struct i2c_client *g_client;

	/**
	 * The I2C device address.
	 * @see{PCA9865_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW} and @see{SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH}
	 */
	uint16_t device_address;
	sht3x_bus_ops_t ops;

} sht3x_handle_t;


void sht3x_init(struct i2c_client *client);

/**
 * Checks if an SHT3x is reachable using the given handle.
 * @param handle Handle to the SHT3x device.
 * @return True on success, false otherwise.
 */
bool sht3x_check_init(void);

/**
 * Takes a single temperature and humidity measurement.
 * @param handle Handle to the SHT3x device.
 * @param temperature Pointer to the storage location for the sampled temperature.
 * @param humidity Pointer to the storage location for the sampled humidity.
 * @return True on success, false otherwise.
 */
bool sht3x_read_temperature_and_humidity(int *temperature, int *humidity);

/**
 * Turns the SHT3x's internal heater on or of.
 * @param handle Handle to the SHT3x device.
 * @param enable True to enable to heater, false to disable it.
 * @return True on success, false otherwise.
 */
bool sht3x_set_header_enable(bool enable);
