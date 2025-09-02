#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include<linux/kdev_t.h>

#include "lcd_i2c_drv.h"

#define LCD1602_NAME "lcd1602"
#define CLASS_NAME "lcd_class"

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
struct lcd_private_drv{
    int total_devices; // Total number of devices
    dev_t dev_num; // This holds the device number
    struct i2c_client *g_client;

    struct class *class_module; // Device class
    struct device *device_module; // Device structure

    struct module_private_data module_data; // Array of module data for each device
};

struct lcd_private_drv lcd_drv = {
    .total_devices = NO_OF_DEVICES,
    .module_data = {
        .serial_number = "001",
    },
};

static ssize_t text_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count)
{
    lcd_send_string(buf);
    return count;
}

static DEVICE_ATTR_WO(text);

static ssize_t set_xy_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count)
{
    int row, col;
    char *kbuf, *token;

    kbuf = kstrdup(buf, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    token = strsep(&kbuf, " ");
    if (!token || kstrtoint(token, 10, &row)) {
        pr_err("lcd1602: invalid row\n");
        goto out;
    }

    token = strsep(&kbuf, " ");
    if (!token || kstrtoint(token, 10, &col)) {
        pr_err("lcd1602: invalid col\n");
        goto out;
    }

    pr_info("lcd1602: set cursor row=%d col=%d\n", row, col);
    lcd_put_cur(row, col);

out:
    kfree(kbuf);
    return count;
}

static DEVICE_ATTR_WO(set_xy);

static ssize_t clear_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count)
{
    lcd_clear();
    return count;
}

static DEVICE_ATTR_WO(clear);

static struct attribute *lcd_attrs[] = {
    &dev_attr_text.attr,
    &dev_attr_set_xy.attr,
    &dev_attr_clear.attr,
    NULL
};
static struct attribute_group lcd_attr_grp ={
    .attrs = lcd_attrs,
    NULL
};

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    // .read           = demo_read,
    // .unlocked_ioctl = i2c_ioctl,
    // .llseek         = no_llseek,
};

static int lcd_sysfs_probe(struct i2c_client *client, const struct i2c_device_id *id){
    int ret;
    lcd_drv.g_client = client;
    
    /* Dynamically allocate memory for the device */
    ret = alloc_chrdev_region(&lcd_drv.dev_num, 0 , 1, LCD1602_NAME);
        if(ret < 0) {
        pr_info("Failed to allocate character device region.\n");
        goto out;
    }

    /* Create a device class */
    lcd_drv.class_module = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(lcd_drv.class_module)){
        pr_info("Failed to create class.\n");
        goto unreg_chrdev;
    }
    
    pr_info("Device created with major: %d, minor: %d\n",\
        MAJOR(lcd_drv.dev_num), MINOR(lcd_drv.dev_num));

    /* Initialize the cdev structure */
    cdev_init(&lcd_drv.module_data.cdev, &fops);

    // 3. Add the cdev to the system with VFS
    lcd_drv.module_data.cdev.owner = THIS_MODULE;
    ret = cdev_add(&lcd_drv.module_data.cdev, \
        lcd_drv.dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add cdev.\n");
        goto class_destroy;
    }

    // 4. Prepare device data
    lcd_drv.device_module = device_create(
                                            lcd_drv.class_module, 
                                            &client->dev,
                                            lcd_drv.dev_num, 
                                            NULL,
                                            LCD1602_NAME);

    if(IS_ERR(lcd_drv.device_module)) {
        printk(KERN_ERR "Failed to create device.\n");
        ret = PTR_ERR(lcd_drv.device_module);
        goto cdev_del;
    }

    ret = sysfs_create_group(&lcd_drv.device_module->kobj, &lcd_attr_grp);
    if (ret) {
        dev_err(lcd_drv.device_module,
                "failed to create sysfs group: %d\n", ret);
        goto cdev_del;
    }
    pr_info("probed: /dev/lcd1602, sysfs in /sys/class/%s/\n",CLASS_NAME);

    lcd_init(lcd_drv.g_client);
    
	// i2c_lcd_put_cur(0,0);
	// i2c_send_string("Init LCD1"); // Test

	// i2c_lcd_put_cur(1,0);
	// i2c_send_string("Init LCD2"); // Test
    return 0;

cdev_del:
    device_destroy(lcd_drv.class_module, lcd_drv.dev_num);
    cdev_del(&lcd_drv.module_data.cdev);
class_destroy:
    class_destroy(lcd_drv.class_module);
unreg_chrdev:
    pr_info("Failed to add cdev.\n");
    unregister_chrdev_region(lcd_drv.dev_num, NO_OF_DEVICES);
out:
    pr_info("Module initialization failed.\n");
    return ret; // Return error code
}

static int lcd_sysfs_remove(struct i2c_client *client){
    sysfs_remove_group(&lcd_drv.device_module->kobj, &lcd_attr_grp);

    device_destroy(lcd_drv.class_module, lcd_drv.dev_num);
    cdev_del(&lcd_drv.module_data.cdev);

    class_destroy(lcd_drv.class_module);

    unregister_chrdev_region(lcd_drv.dev_num, NO_OF_DEVICES);

    pr_info("i2c device driver removed.\n");
    return 0;
}

static struct i2c_device_id lcd_id[]={
    {.name = LCD1602_NAME, .driver_data = 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, lcd_id);

static const struct of_device_id lcd_of_match[] = {
    { .compatible = "org, lcd1602"},
    {}
};
MODULE_DEVICE_TABLE(of, lcd_of_match);

static struct i2c_driver lcd_driver = {
    .driver = {
        .name = LCD1602_NAME,
        .of_match_table = lcd_of_match,
    },
    .probe = lcd_sysfs_probe,
    .remove = lcd_sysfs_remove,
    .id_table = lcd_id,
};


/* Init and Exit module*/
static int __init __my_module_driver_init(void){
    pr_info("Module init\n");
    return i2c_add_driver(&lcd_driver);
}

static void __exit __my_module_driver_exit(void){
    i2c_del_driver(&lcd_driver);
    lcd_clear();
    pr_info("Module exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phuoc");
MODULE_DESCRIPTION("I2C driver with /dev + sysfs");
