#include"module_n.h"

struct module_private_data_driver module_private_data = {
    .total_devices = NO_OF_DEVICES,
    .module_data = {
        [0] = {
            .buff = device_buff_mod1,
            .size = MEM_MAX_MOD_1,
            .serial_number = "N001",
            .perm = RDONLY // Read permissions
        },
        [1] = {
            .buff = device_buff_mod2,
            .size = MEM_MAX_MOD_2,
            .serial_number = "N002",
            .perm = WRONLY // Write permissions
        },
        [2] = {
            .buff = device_buff_mod3,
            .size = MEM_MAX_MOD_3,
            .serial_number = "N003",
            .perm = RDWR // Read Write permissions
        },
        [3] = {
            .buff = device_buff_mod4,
            .size = MEM_MAX_MOD_4,
            .serial_number = "N004",
            .perm = RDWR // Read Write permissions
        }
    },
};

loff_t module_lseek(struct file *flip, loff_t off, int whence){
    return 0;
}

ssize_t module_read(struct file *flip, char __user *buff, size_t count, loff_t *f_pos){
    return 0;
}

ssize_t module_write(struct file *flip, const char __user *buff, size_t count, loff_t *f_pos){
    return -ENOMEM; 
}

int Check_Permissions(int perm, int minor_n, int flags){
    pr_info("%s : Checking permissions for minor number: %d\n", __func__, minor_n);
    
    if(perm == RDWR){
        return 0;
    }

    pr_info("%s : flags: %d, RFMODE_READ: %d, FMODE_WRITE: %d\n", __func__, flags,FMODE_READ , FMODE_WRITE);
    // Read permissions
    if((perm == RDONLY) && ((flags & FMODE_READ) && !(flags & FMODE_WRITE))){
        pr_info("%s : Read permissions granted for minor number: %d\n", __func__, minor_n);
        return 0;
    }

    // Write permissions
    if((perm == WRONLY) && (!(flags & FMODE_READ) && (flags & FMODE_WRITE))){
        pr_info("%s : Write permissions granted for minor number: %d\n", __func__, minor_n);
        return 0;
    }
    pr_info("%s : Permission denied for minor number: %d\n", __func__, minor_n);
    return -EPERM; // Return error code for permission denied
}

int module_open(struct inode *inode, struct file *flip){
    return 0;
}

int module_release(struct inode *inode, struct file *flip){
    pr_info("%s : Device closed.\n", __func__);
    return 0; // Return 0 to indicate successful closing
}

// File operations of the driver
struct file_operations module_fops = {
    .open = module_open,
    .release = module_release,
    .read = module_read,
    .write = module_write,
    .llseek = module_lseek,
    .owner = THIS_MODULE
};


/* Call when matched platform device is found */
int Module_platform_driver_probe(struct platform_device *pdev){
    struct device_private_data *dev_data;
    struct Module_platform_data *platform_data;
    
    int ret;
    pr_info("%s : Device is detected\n", __func__);
    /* 1. Get the platform data*/
    platform_data = (struct Module_platform_data*)dev_get_platdata(&pdev->dev);
    if(!platform_data){
        pr_info("%s : No platform data availble\n", __func__);
        ret = -EINVAL;
        goto out;
    }

    /* 2. Dynamic allocate memory for the device platform driver*/
    dev_data = kzalloc(sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data){
        pr_info("%s : Cannot alloccate memory\n", __func__);
        ret = -ENOMEM;
        goto out;
    }

    /* Set device_private_data pointer in platform device structure */
    dev_set_drvdata(&pdev->dev, dev_data);

    dev_data->pdata.size = platform_data->size;
    dev_data->pdata.perm = platform_data->perm;
    dev_data->pdata.serial_number = platform_data->serial_number;

    pr_info("%s : Device serial number = %s\n", __func__, dev_data->pdata.serial_number);
    pr_info("%s : Device size = %d\n", __func__, dev_data->pdata.size);
    pr_info("%s : Device permission = %d\n", __func__, dev_data->pdata.perm);
    
    /* 3. Dynamic allocate memory for the device buffer using size*/
    dev_data->buff = kzalloc(dev_data->pdata.size, GFP_KERNEL);
    if(!dev_data->buff){
        pr_info("%s : Cannot alloccate memory for buffer\n", __func__);
        ret = -ENOMEM;
        goto dev_data_free;
    }

    /* 4. Get the device number*/
    dev_data->dev_num = drv_data.dev_num_base + pdev->id;

    /* 5. cdev init and cdev add*/
    cdev_init(&dev_data->cdev, &module_fops);
    dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->cdev,
                    dev_data->dev_num, 1);
        if (ret < 0) {
            printk(KERN_ERR "Failed to add cdev.\n");
            goto buffer_free;
        }

    /* 6. Create device file for the detected platform device */
    drv_data.device_module = device_create(
                                            drv_data.class_module, 
                                            NULL,
                                            dev_data->dev_num, 
                                            NULL,
                                            "my_device-%d", pdev->id);
    if(IS_ERR(drv_data.device_module)) {
        printk(KERN_ERR "Failed to create device.\n");
        ret = PTR_ERR(drv_data.device_module);
        goto cdev_del;
    }

    drv_data.total_devices++;

    pr_info("%s : The probe was successful\n", __func__);
    return 0;

cdev_del:
    cdev_del(&dev_data->cdev);

buffer_free:
    kfree(dev_data->buff);
    return ret;

dev_data_free:
    kfree(dev_data);
    return ret;

out: 
    pr_info("%s : Device probe failed\n", __func__);
    return ret;
}

/* Call when the device is removed from the system */
int Module_platform_driver_remove(struct platform_device *pdev){
    pr_info("%s : Module is removed\n", __func__);
    
    struct device_private_data *dev_data = dev_get_drvdata(&pdev->dev);
    device_destroy(drv_data.class_module, dev_data->dev_num);
    cdev_del(&dev_data->cdev);
    kfree(dev_data);
    kfree(dev_data->buff);
    drv_data.total_devices--;
    pr_info("%s : Device probe remove\n", __func__);

    return 0;
}


struct platform_driver Module_platform_driver = {
    .probe = Module_platform_driver_probe,
    .remove = Module_platform_driver_remove,
    .driver = {
        .name = "char_device"
    }
};

static int __init __my_module_driver_init(void){
    int ret;
    /* 1. Dynamic allocate a device number for MAX_DEVICES */
    ret = alloc_chrdev_region(&drv_data.dev_num_base, 0, MAX_DEVICES, "module");
    if (ret < 0){
        pr_err("Allocate driver failed");
        return ret;
    }

    /* 2. Create device class under /sys/class */
    drv_data.class_module = class_create("device_class");
    if(IS_ERR(drv_data.class_module)) {
        printk(KERN_ERR "Failed to create device class.\n");
        ret = PTR_ERR(drv_data.class_module);
        unregister_chrdev_region(drv_data.dev_num_base, MAX_DEVICES);
        return ret;
    }

    /* 3. Register a platform driver */
    platform_driver_register(&Module_platform_driver);
    pr_info("%s : Module driver init", __func__);
    return 0; // Return 0 to indicate successful loading
}

static void __exit __my_module_driver_exit(void){
    platform_driver_unregister(&Module_platform_driver);
    class_destroy(drv_data.class_module);
    unregister_chrdev_region(drv_data.dev_num_base, MAX_DEVICES);
    pr_info("%s : Module driver exit", __func__);
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("002");
MODULE_DESCRIPTION("A simple module driver for a pseudo character device");
MODULE_INFO(boarder, "Module001");
