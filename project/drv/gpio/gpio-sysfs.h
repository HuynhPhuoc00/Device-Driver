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
#include<linux/version.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

struct gpio_dev_private_data{
    char label[20];
    struct gpio_desc *desc;
};

struct gpio_drv_private_data{
    int total_devices;
    struct class *class_gpio;
    struct device **dev_mod;
};

ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t direction_store(struct device *dev, struct device_attribute *attr,
                        const char *buf, size_t count);
ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t value_store(struct device *dev, struct device_attribute *attr,
                        const char *buf, size_t count);
ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf);

int gpio_sysfs_probe(struct platform_device *pdev);
int gpio_sysfs_remove(struct platform_device *pdev);

#endif // GPIO_SYSFS_H