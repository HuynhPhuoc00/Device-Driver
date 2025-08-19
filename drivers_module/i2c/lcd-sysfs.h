#ifndef GPIO_SYSFS_H
#define GPIO_SYSFS_H

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/platform_device.h>
#include<linux/string.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include<linux/of_gpio.h>
#include<linux/sysfs.h>
#include<linux/gpio/consumer.h>
#include<linux/property.h>
#include <linux/i2c.h>

#include<linux/version.h>

#include "lcd_i2c_drv.h"

 
#define LCD1602_NAME "lcd1602"
#define LCD_I2C_ADDR 0x27

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

struct lcd_dev_private_data{
    char label[20];
    struct lcd_desc *desc;
};

struct lcd_drv_private_data{
    int total_devices;
    struct class *lcd_class;
    struct device *lcd_device;
    struct i2c_client *client;
};

static ssize_t text_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count);
int lcd_sysfs_probe(struct i2c_client *client, const struct i2c_device_id *id);
int lcd_sysfs_remove(struct i2c_client *client);

#endif // GPIO_SYSFS_H