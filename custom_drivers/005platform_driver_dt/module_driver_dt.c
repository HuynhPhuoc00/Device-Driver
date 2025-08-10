#include"module_n.h"

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
    pr_info("Checking permissions for minor number: %d\n", minor_n);
    
    if(perm == RDWR){
        return 0;
    }

   // Read permissions
    if((perm == RDONLY) && ((flags & FMODE_READ) && !(flags & FMODE_WRITE))){
        pr_info("Read permissions granted for minor number: %d\n", minor_n);
        return 0;
    }

    // Write permissions
    if((perm == WRONLY) && (!(flags & FMODE_READ) && (flags & FMODE_WRITE))){
        pr_info("Write permissions granted for minor number: %d\n", minor_n);
        return 0;
    }
    pr_info("Permission denied for minor number: %d\n", minor_n);
    return -EPERM; // Return error code for permission denied
}

int module_open(struct inode *inode, struct file *flip){
    return 0;
}

int module_release(struct inode *inode, struct file *flip){
    pr_info("Device closed.\n");
    return 0; // Return 0 to indicate successful closing
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

// File operations of the driver
struct file_operations module_fops = {
    .open = module_open,
    .release = module_release,
    .read = module_read,
    .write = module_write,
    .llseek = module_lseek,
    .owner = THIS_MODULE
};

/* Match for platform driver with device setup*/
struct platform_driver Module_platform_driver = {
    .probe = Module_platform_driver_probe,
    .remove = Module_platform_driver_remove,
    .id_table = device_id,
    .driver = {
        .name = "mod",
        .of_match_table = of_match_ptr(mod_dt_match) 
    }
};

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
                                            "my_device-%d", drv_data.total_devices);
    if(IS_ERR(drv_data.device_module)) {
        dev_err(dev, "Failed to create device.\n");
        ret = PTR_ERR(drv_data.device_module);
        cdev_del(&dev_data->cdev);
        return ret;
    }

    drv_data.total_devices++;

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
    ret = alloc_chrdev_region(&drv_data.dev_num_base, 0, MAX_DEVICES, "mod");
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
