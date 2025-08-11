#include"module_driver_dt_sysfs.h"

// File operations of the driver
struct file_operations module_fops = {
    .llseek = module_lseek,
    .read = module_read,
    .write = module_write,
    .open = module_open,
    .release = module_release,
    .owner = THIS_MODULE
};

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


/* Match for platform driver with device setup*/
struct platform_driver Module_platform_driver = {
    .probe = Module_platform_driver_probe,
    .remove = Module_platform_driver_remove,
    .id_table = device_id,
    .driver = {
        .name = "moddev",
        .of_match_table = of_match_ptr(mod_dt_match) 
    }
};

struct driver_private_data drv_data;

ssize_t show_max_size(struct device *dev, struct device_attribute * attr, char *buf){
    /* Get acces from dev private data*/
    struct device_private_data *dev_data = dev_get_drvdata(dev->parent);

    return sprintf(buf, "%d\n", dev_data->pdata.size);
}
ssize_t store_max_size(struct device *dev, struct device_attribute * attr, const char *buf, size_t count){
    long res;
    int ret;
    ret = kstrtol(buf,10,&res);
    struct device_private_data *dev_data = dev_get_drvdata(dev->parent);
    if(ret){
        return ret;
    }
    dev_data->pdata.size = res;
    dev_data->buff = krealloc(dev_data->buff, dev_data->pdata.size, GFP_KERNEL);
    return count;
}

ssize_t show_serial_num(struct device *dev, struct device_attribute * attr, char *buf){
    /* Get acces from dev private data*/
    struct device_private_data *dev_data = dev_get_drvdata(dev->parent);

    return sprintf(buf, "%s\n", dev_data->pdata.serial_number);
}

/* Device actribute var*/
static DEVICE_ATTR(max_size, S_IRUGO|S_IWUSR, show_max_size, store_max_size);
static DEVICE_ATTR(serial_num, S_IRUGO, show_serial_num, NULL);

int mod_sysfs_create_file(struct device *mod_dev){
    int ret;
    ret = sysfs_create_file(&mod_dev->kobj, &dev_attr_max_size.attr);
    if(ret){
        return ret;
    }
    return sysfs_create_file(&mod_dev->kobj, &dev_attr_serial_num.attr);
}


struct Module_platform_data* dev_get_platdata_from_dt(struct device *dev){
    struct device_node *dev_node = dev->of_node;
    struct Module_platform_data *platform_data;

    if(!dev_node){
        /*Probe didnt happen because of device tree node*/
        return NULL;
    }
    platform_data = devm_kzalloc(dev, sizeof(*platform_data), GFP_KERNEL);
    if(!platform_data){
        dev_info(dev, "Cannot allocate memory\n");
        return ERR_PTR(-ENOMEM);
    }

    if(of_property_read_string(dev_node, "org,device-serial-num", &platform_data->serial_number)){
        dev_info(dev, "Missing serial number property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,size", &platform_data->size)){
        dev_info(dev, "Missing size property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,perm", &platform_data->perm)){
        dev_info(dev, "Missing permission property\n");
        return ERR_PTR(-EINVAL);
    }
    return platform_data;
}

/* Call when matched platform device is found */
int Module_platform_driver_probe(struct platform_device *pdev){
    struct device_private_data *dev_data;
    struct Module_platform_data *platform_data;
    struct device *dev;
    /* Use to store ddected matched entry of "of_device_id" list of this driver */
    const struct of_device_id *match;
    

    int driver_data;
    int ret;
    dev = &pdev->dev;
    dev_info(dev, "Device is detected\n");

    /* Match will always be NULL if linux doesnt support device tree or CONFIG_OF is off*/
    match = of_match_device(of_match_ptr(mod_dt_match), dev);
    if(match){
        platform_data = dev_get_platdata_from_dt(dev);
            if(IS_ERR(platform_data)){
        return -EINVAL;
        }
    }
    else{
        platform_data = (struct Module_platform_data*)dev_get_platdata(dev);
        driver_data = pdev->id_entry->driver_data;
    }

    if(!platform_data){
        dev_info(dev, "No platform data availble\n");
        return -EINVAL;
    }
 

    /* 2. Dynamic allocate memory for the device platform driver*/
    dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data){ 
        dev_info(dev, "Cannot alloccate memory\n");
        return -ENOMEM;
    }

    /* Set device_private_data pointer in platform device structure */
    dev_set_drvdata(dev, dev_data);

    dev_data->pdata.size = platform_data->size;
    dev_data->pdata.perm = platform_data->perm;
    dev_data->pdata.serial_number = platform_data->serial_number;

    pr_info("Device serial number = %s\n", dev_data->pdata.serial_number);
    pr_info("Device size = %d\n", dev_data->pdata.size);
    pr_info("Device permission = %d\n", dev_data->pdata.perm);
    
    pr_info("Config item1 = %d\n", Device_config[driver_data].config_item1);
    pr_info("Config item2 = %d\n", Device_config[driver_data].config_item2);
    
    /* 3. Dynamic allocate memory for the device buffer using size*/
    dev_data->buff = devm_kzalloc(dev, dev_data->pdata.size, GFP_KERNEL);
    if(!dev_data->buff){
        dev_info(dev, "Cannot alloccate memory for buffer\n");
        return -ENOMEM;
    }

    /* 4. Get the device number*/
    dev_data->dev_num = drv_data.dev_num_base + drv_data.total_devices;

    /* 5. cdev init and cdev add*/
    cdev_init(&dev_data->cdev, &module_fops);
    dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->cdev,
                    dev_data->dev_num, 1);
        if (ret < 0) {
            dev_err(dev, "Failed to add cdev.\n");
            return ret;
        }

    /* 6. Create device file for the detected platform device 
     *   Store at /dev/
    */

    drv_data.device_module = device_create(
                                            drv_data.class_module, 
                                            dev,
                                            dev_data->dev_num, 
                                            NULL,
                                            "moddev-%d", drv_data.total_devices);
    if(IS_ERR(drv_data.device_module)) {
        dev_err(dev, "Failed to create device.\n");
        ret = PTR_ERR(drv_data.device_module);
        cdev_del(&dev_data->cdev);
        return ret;
    }

    drv_data.total_devices++;

    ret = mod_sysfs_create_file(drv_data.device_module);
    if(ret){
        device_destroy(drv_data.class_module, dev_data->dev_num);
        return ret;
    }

    dev_info(dev, "The probe was successful\n");
    return 0; 
}

/* Call when the device is removed from the system */
int Module_platform_driver_remove(struct platform_device *pdev){
    struct device_private_data *dev_data;
    
    dev_data = dev_get_drvdata(&pdev->dev);
    device_destroy(drv_data.class_module, dev_data->dev_num);
    cdev_del(&dev_data->cdev);
    drv_data.total_devices--;

    dev_info(&pdev->dev, "Device probe remove\n");
    return 0;
}

static int __init __my_module_driver_init(void){
    int ret;
    
    /* 1. Dynamic allocate a device number for MAX_DEVICES */
    ret = alloc_chrdev_region(&drv_data.dev_num_base, 0, MAX_DEVICES, "moddev");
    if (ret < 0){
        pr_err("Allocate driver failed");
        return ret;
    }

    /* 2. Create device class under /sys/class */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,12,0)
    drv_data.class_module = class_create("device_class");
#else
    drv_data.class_module = class_create(THIS_MODULE, "device_class");
#endif
    if(IS_ERR(drv_data.class_module)) {
        pr_info("Failed to create device class.\n");
        ret = PTR_ERR(drv_data.class_module);
        unregister_chrdev_region(drv_data.dev_num_base, MAX_DEVICES);
        return ret;
    }

    /* 3. Register a platform driver */
    platform_driver_register(&Module_platform_driver);

    pr_info("Module driver init\n");
    return 0; // Return 0 to indicate successful loading
}

static void __exit __my_module_driver_exit(void){
    platform_driver_unregister(&Module_platform_driver);
    class_destroy(drv_data.class_module);
    unregister_chrdev_region(drv_data.dev_num_base, MAX_DEVICES);
    pr_info("Module driver exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

// /* Create platform driver include init and exit */
// module_platform_driver(Module_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("005");
MODULE_DESCRIPTION("A simple module driver for platform devices");
MODULE_INFO(boarder, "Platform Devices");
