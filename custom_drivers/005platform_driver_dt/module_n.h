#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include <linux/version.h>
#include"platform.h"

#define RDONLY  0x01 // Read permissions
#define WRONLY  0x10 // Write permissions
#define RDWR    0x11 // Read Write permissions 

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

struct driver_private_data drv_data;

loff_t module_lseek(struct file *filp, loff_t off, int whence);
ssize_t module_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t module_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int module_open(struct inode *inode, struct file *filp);
int module_release(struct inode *inode, struct file *filp);
int Check_Permissions(int perm, int minor_n, int flags);

int Module_platform_driver_probe(struct platform_device *pdev);
int Module_platform_driver_remove(struct platform_device *pdev);