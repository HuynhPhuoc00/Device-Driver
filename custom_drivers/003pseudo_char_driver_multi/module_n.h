#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define RDONLY  0x01 // Read permissions
#define WRONLY  0x10 // Write permissions
#define RDWR    0x11 // Read Write permissions 

#define NO_OF_DEVICES 4 // Number of devices

#define MEM_MAX_MOD_1 1024// Size of the device memory
#define MEM_MAX_MOD_2 1024// Size of the device memory
#define MEM_MAX_MOD_3 1024// Size of the device memory
#define MEM_MAX_MOD_4 1024// Size of the device memory

// Buffer to hold device data
char device_buff_mod1[MEM_MAX_MOD_1];
char device_buff_mod2[MEM_MAX_MOD_2];
char device_buff_mod3[MEM_MAX_MOD_3];
char device_buff_mod4[MEM_MAX_MOD_4];

// Struct private data
struct module_private_data {
    char *buff;
    unsigned size;
    const char *serial_number;
    int perm;
    struct cdev cdev; // Character device structure
};

// Driver private data structure
struct module_private_data_driver
{
    int total_devices; // Total number of devices
    dev_t dev_num; // This holds the device number

    struct class *class_module; // Device class
    struct device *device_module; // Device structure

    struct module_private_data module_data[NO_OF_DEVICES]; // Array of module data for each device
};

loff_t module001_lseek(struct file *filp, loff_t off, int whence);
ssize_t module001_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t module001_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int module001_open(struct inode *inode, struct file *filp);
int module001_release(struct inode *inode, struct file *filp);
int Check_Permissions(int perm, int minor_n, int flags);