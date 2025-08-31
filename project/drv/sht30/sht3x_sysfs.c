#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include<linux/kdev_t.h>

#include "sht3x.h"

#define SHT_NAME "sht30"
#define CLASS_NAME "sht_class"

#define NO_OF_DEVICES 1

// Struct private data
struct module_private_data {
    char *buff;
    unsigned size;
    const char *serial_number;
    int perm;
    struct cdev cdev; // Character device structure
};

// Driver private data structure
struct i2c_private_drv
{
    int total_devices; // Total number of devices
    dev_t dev_num; // This holds the device number
    struct i2c_client *g_client;

    struct class *class_module; // Device class
    struct device *device_module; // Device structure

    struct module_private_data module_data; // Array of module data for each device
};

struct i2c_private_drv sht_drv = {
    .total_devices = NO_OF_DEVICES,
    .module_data = {
        .serial_number = "sh3x_1",
    },
};

/* Device actribute var*/
static ssize_t readSht_show(struct device *dev,
                          struct device_attribute *attr,
                          char *buf)
{
    int temperature, humidity;
    int ret;
    ssize_t len;

    ret = sht3x_read_temperature_and_humidity(&temperature, &humidity);
    if(ret < 0){
        pr_info("sht: initial read failed (%d)\n", ret);
        return scnprintf(buf, PAGE_SIZE, "error: read failed\n");
    }
    pr_info("Initial temperature: %.2dC, humidity: %.2d%%RH\n\r", temperature, humidity);

    // Enable heater for two seconds.
    ret = sht3x_set_header_enable(true); /* giả định API */
    if (ret < 0)
        pr_info("sht: enable heater failed (%d)\n", ret);
    msleep(2000);
    sht3x_set_header_enable(false);

    // Read temperature and humidity again.
    ret = sht3x_read_temperature_and_humidity(&temperature, &humidity);
    if (ret < 0) {
        pr_info("sht: second read failed (%d)\n", ret);
        return scnprintf(buf, PAGE_SIZE, "error: read failed\n");
    }
    pr_info("After heating temperature: %.2dC, humidity: %.2d%%RH\n\r", temperature, humidity);

    len = scnprintf(buf, PAGE_SIZE, "Humidity = %.2d, Temp = %.2d\n", humidity, temperature);
    return len;
}

static DEVICE_ATTR_RO(readSht);

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    // .read           = sht_read,
    // .unlocked_ioctl = i2c_ioctl,
    // .llseek         = no_llseek,
};

static int sht_sysfs_probe(struct i2c_client *client, const struct i2c_device_id *id){
    int ret;
    sht_drv.g_client = client;
    
    /* Dynamically allocate memory for the device */
    ret = alloc_chrdev_region(&sht_drv.dev_num, 0 , NO_OF_DEVICES, SHT_NAME);
        if(ret < 0) {
        pr_info("Failed to allocate character device region.\n");
        goto out;
    }

    /* Create a device class */
    sht_drv.class_module = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(sht_drv.class_module)){
        pr_info("Failed to create class.\n");
        goto unreg_chrdev;
    }
    
    pr_info("Device created with major: %d, minor: %d\n",\
        MAJOR(sht_drv.dev_num), MINOR(sht_drv.dev_num));

    /* Initialize the cdev structure */
    cdev_init(&sht_drv.module_data.cdev, &fops);

    // 3. Add the cdev to the system with VFS
    sht_drv.module_data.cdev.owner = THIS_MODULE;
    ret = cdev_add(&sht_drv.module_data.cdev, \
        sht_drv.dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add cdev.\n");
        goto class_destroy;
    }

    // 4. Prepare device data
    sht_drv.device_module = device_create(
                                            sht_drv.class_module, 
                                            &client->dev,
                                            sht_drv.dev_num, 
                                            NULL,
                                            SHT_NAME);

    if(IS_ERR(sht_drv.device_module)) {
        printk(KERN_ERR "Failed to create device.\n");
        ret = PTR_ERR(sht_drv.device_module);
        goto cdev_del;
    }

    ret = device_create_file(sht_drv.device_module, &dev_attr_readSht);
    if (ret) {
        pr_info("Failed to create sysfs attr: %d\n", ret);
        goto device_destroy;
    }
    pr_info("probed: /dev/sht30, sysfs in /sys/class/%s/\n",CLASS_NAME);

    /* Init sensor*/
    sht3x_init(client);

    return 0;

device_destroy:
    device_destroy(sht_drv.class_module, sht_drv.dev_num);
cdev_del:
    cdev_del(&sht_drv.module_data.cdev);
class_destroy:
    class_destroy(sht_drv.class_module);
unreg_chrdev:
    pr_info("Failed to add cdev.\n");
    unregister_chrdev_region(sht_drv.dev_num, NO_OF_DEVICES);
out:
    pr_info("Module initialization failed.\n");
    return ret; // Return error code
}

static int sht_sysfs_remove(struct i2c_client *client){
    if(sht_drv.device_module){
        device_remove_file(sht_drv.device_module, &dev_attr_readSht);
    }

    device_destroy(sht_drv.class_module, sht_drv.dev_num);
    cdev_del(&sht_drv.module_data.cdev);

    class_destroy(sht_drv.class_module);

    unregister_chrdev_region(sht_drv.dev_num, NO_OF_DEVICES);

    pr_info("sht device driver removed.\n");
    return 0;
}


static struct i2c_device_id sht_id[]={
    {.name = SHT_NAME, .driver_data = 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, sht_id);

static const struct of_device_id sht_of_match[] = {
    { .compatible = "org, sht3x"},
    {}
};
MODULE_DEVICE_TABLE(of, sht_of_match);

static struct i2c_driver sht_driver = {
    .driver = {
        .name = SHT_NAME,
        .of_match_table = sht_of_match,
    },
    .probe = sht_sysfs_probe,
    .remove = sht_sysfs_remove,
    .id_table = sht_id,
};

/* Init and Exit module*/
static int __init __my_module_driver_init(void){
    pr_info("Module init\n");
    return i2c_add_driver(&sht_driver);
}

static void __exit __my_module_driver_exit(void){
    i2c_del_driver(&sht_driver);
    pr_info("Module exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phuoc");
MODULE_DESCRIPTION("I2C driver with /dev + sysfs");