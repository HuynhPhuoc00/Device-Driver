#ifndef MODULE_DRIVER_DT_SYSFS_H
#define MODULE_DRIVER_DT_SYSFS_H

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include <linux/version.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include<linux/sysfs.h>
#include"platform.h"

#define RDONLY  0x01 // Read permissions
#define WRONLY  0x10 // Write permissions
#define RDWR    0x11 // Read Write permissions 

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

/* Device private data*/
struct device_private_data{
    struct Module_platform_data pdata;
    char *buff;
    dev_t dev_num;
    struct cdev cdev;
};

/* Driver private data*/
struct driver_private_data{
    int total_devices;
    dev_t dev_num_base;
    struct class *class_module;
    struct device *device_module;
};

enum mod_names{
    moddev_A = 0,
    moddev_B = 1,
    moddev_C = 2,
    moddev_D = 3
};

struct device_config{
	int config_item1;
	int config_item2;
};

/*Device_attribure*/
// struct

int Module_platform_driver_probe(struct platform_device *pdev);
int Module_platform_driver_remove(struct platform_device *pdev);


loff_t module_lseek(struct file *filp, loff_t off, int whence);
ssize_t module_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t module_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int module_open(struct inode *inode, struct file *filp);
int module_release(struct inode *inode, struct file *filp);
int Check_Permissions(int perm, int minor_n, int flags);
struct Module_platform_data* dev_get_platdata_from_dt(struct device *dev);


/* Device actribute */
ssize_t show_max_size(struct device *dev, struct device_attribute * attr, char *buf);
ssize_t store_max_size(struct device *dev, struct device_attribute * attr, const char *buf, size_t count);
ssize_t show_serial_num(struct device *dev, struct device_attribute * attr, char *buf);

int mod_sysfs_create_file(struct device *mod_dev);

#endif // MODULE_DRIVER_DT_SYSFS_H