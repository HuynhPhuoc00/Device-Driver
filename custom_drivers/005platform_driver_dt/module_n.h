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

struct driver_private_data drv_data;

struct platform_device_id device_id[] = {
    [moddev_A] = {
        .name = "moddev_A"
    },
    [moddev_B] = {
        .name = "moddev_B"
    },
    [moddev_C] = {
        .name = "moddev_C"
    },
    [moddev_D] = {
        .name = "moddev_D"
    }
};

struct device_config Device_config[] = 
{
	[moddev_A] = {.config_item1 = 60, .config_item2 = 21},
	[moddev_B] = {.config_item1 = 50, .config_item2 = 22},
	[moddev_C] = {.config_item1 = 40, .config_item2 = 23},
	[moddev_D] = {.config_item1 = 30, .config_item2 = 24}
};

/* Match for platform driver with device tree */
struct of_device_id mod_dt_match[] = {
    [moddev_A] = {.compatible = "moddev_A", .data = (void*)moddev_A},
    [moddev_B] = {.compatible = "moddev_B", .data = (void*)moddev_B},
    [moddev_C] = {.compatible = "moddev_C", .data = (void*)moddev_C},
    [moddev_D] = {.compatible = "moddev_D", .data = (void*)moddev_D}
};

loff_t module_lseek(struct file *filp, loff_t off, int whence);
ssize_t module_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t module_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int module_open(struct inode *inode, struct file *filp);
int module_release(struct inode *inode, struct file *filp);
int Check_Permissions(int perm, int minor_n, int flags);
struct Module_platform_data* dev_get_platdata_from_dt(struct device *dev);

int Module_platform_driver_probe(struct platform_device *pdev);
int Module_platform_driver_remove(struct platform_device *pdev);