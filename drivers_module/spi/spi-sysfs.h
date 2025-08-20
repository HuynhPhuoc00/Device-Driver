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
#include <linux/spi/spi.h>

#include<linux/version.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#define DEV_NAME ""
struct dev_private_data{
    char label[20];
    struct lcd_desc *desc;
};

struct drv_private_data{
    int total_devices;
    struct class *dev_class;
    struct device *dev_device;
    struct spi_client *client;
};

static ssize_t text_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count);
int spi_sysfs_probe(struct spi_device *spi);
int spi_sysfs_remove(struct spi_device *spi);

#endif // GPIO_SYSFS_H