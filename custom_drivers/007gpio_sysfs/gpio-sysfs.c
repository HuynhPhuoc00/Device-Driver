#include"gpio-sysfs.h"

struct gpio_drv_private_data gpio_drv_data;

struct of_device_id gpio_device_id[]={
    [0] = {
        .compatible = "org,gpio-sysfs",
    }
};

struct platform_driver gpio_platform_drv = {
    .probe = gpio_sysfs_probe,
    .remove = gpio_sysfs_remove,
    .driver = {
        .name = "GPIOS",
        .of_match_table = of_match_ptr(gpio_device_id)
    }
};

ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf){
    struct gpio_dev_private_data *gpio_dev_data;

    int dir;
    char *direction;

    gpio_dev_data = dev_get_drvdata(dev);
    
    dir = gpiod_get_direction(gpio_dev_data->desc);
    if(dir < 0) return dir;
    direction = (dir == 0) ? "out" : "in";
    
    return sprintf(buf, "%s\n", direction);
} 

ssize_t direction_store(struct device *dev, struct device_attribute *attr,
                        const char *buf, size_t count){
    struct gpio_dev_private_data *gpio_dev_data;
    int ret;

    gpio_dev_data = dev_get_drvdata(dev);
    if(sysfs_streq(buf, "in")){
        ret = gpiod_direction_input(gpio_dev_data->desc);
    }
    else if(sysfs_streq(buf, "out")){
        ret = gpiod_direction_output(gpio_dev_data->desc,0);
    }
    else ret = -EINVAL;
    return ret ? : count;
}

ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf){
    struct gpio_dev_private_data *gpio_dev_data;
    int value;

    gpio_dev_data = dev_get_drvdata(dev);    
    value = gpiod_get_value(gpio_dev_data->desc);

    return sprintf(buf,"%d\n", value);
} 

ssize_t value_store(struct device *dev, struct device_attribute *attr,
                        const char *buf, size_t count){
    struct gpio_dev_private_data *gpio_dev_data;
    int ret;
    long value;

    gpio_dev_data = dev_get_drvdata(dev);
    ret = kstrtol(buf, 0, &value);
    if(ret) return ret;

    gpiod_set_value(gpio_dev_data->desc, value);
    
    return count;
} 

ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf){
    struct gpio_dev_private_data *gpio_dev_data;
    gpio_dev_data = dev_get_drvdata(dev);
    return sprintf(buf, "%s\n", gpio_dev_data->label);
} 


static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

static struct attribute *gpio_attrs[] = {
    &dev_attr_direction.attr,
    &dev_attr_value.attr,
    &dev_attr_label.attr,
    NULL
};
static struct attribute_group gpio_attr_grp ={
    .attrs = gpio_attrs
};

static const struct attribute_group *gpio_attr_grps[] ={
    &gpio_attr_grp,
    NULL
};

/* Get and put GPIO */
int gpio_sysfs_probe(struct platform_device *pdev){
    struct device *dev;
    struct device_node *parent;
    struct device_node *child;
    struct gpio_dev_private_data *dev_data;
    struct device *dev_ptr;


    const char *name;
    int i = 0;
    int ret;
    dev = &pdev->dev;
    parent = pdev->dev.of_node;
    child = NULL;
    
    gpio_drv_data.total_devices = of_get_child_count(parent);
    if(!gpio_drv_data.total_devices){
        dev_err(dev, "No devices found\n");
        return -EINVAL;
    }
    dev_info(dev, "total_devices found = %d\n", gpio_drv_data.total_devices);
    gpio_drv_data.dev_mod = devm_kzalloc(dev, sizeof(struct device*) *(gpio_drv_data.total_devices), GFP_KERNEL);


    for_each_available_child_of_node(parent, child){
        dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
        if(!dev_data){
            dev_err(dev, "Cannot alloccate memory\n");
            return -ENOMEM;
        }
        if(of_property_read_string(child, "label", &name)){
            dev_warn(dev, "Missing label infor\n");
            snprintf(dev_data->label, sizeof(dev_data->label), "unknown gpio%d",i);
        }
        else{
            strcpy(dev_data->label, name);
            dev_info(dev, "GPIO label = %s\n",dev_data->label);
        }


        dev_data->desc = devm_fwnode_get_gpiod_from_child(dev, 
                                                        "bone", 
                                                        &child->fwnode,
                                                        GPIOD_ASIS,
                                                        dev_data->label);

        if(IS_ERR(dev_data->desc)){
            ret = PTR_ERR(dev_data->desc);
            if(ret == -ENOENT){
                dev_err(dev, "No GPIO has been assigned to the request func and/or index\n");
                return ret;
            }
            else{
                return ret;
            }
        }

        /* Set GPIO to output */
        ret = gpiod_direction_output(dev_data->desc, 0);
        if(ret){
            dev_err(dev,"GPIO direction set failed\n");
            return ret;
        }

        /* Create devices under /sys/class/gpios */
        gpio_drv_data.dev_mod[i] = device_create_with_groups(gpio_drv_data.class_gpio,
                                    dev, 
                                    0,
                                    dev_data,
                                    gpio_attr_grps,
                                    dev_data->label);
        if(IS_ERR(gpio_drv_data.dev_mod[i])){
            dev_err(dev,"Error in device_create\n");
            return PTR_ERR(dev_ptr);
        }
        i++;
    }
    return 0;
}

int gpio_sysfs_remove(struct platform_device *pdev){
    int i;
    dev_info(&pdev->dev, "Remove device");

    for(i = 0; i < gpio_drv_data.total_devices; i++){
        device_unregister(gpio_drv_data.dev_mod[i]);
    }
    return 0;
}

/* Init and Exit module*/
static int __init __my_module_driver_init(void){
    /* Create class for module*/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,12,0)
    gpio_drv_data.class_gpio = class_create("gpios_class");
#else
    gpio_drv_data.class_gpio = class_create(THIS_MODULE, "gpios");
#endif

    /* Register platform driver */
    platform_driver_register(&gpio_platform_drv);

    pr_info("Module init\n");
    return 0; // Return 0 to indicate successful loading
}

static void __exit __my_module_driver_exit(void){
    platform_driver_unregister(&gpio_platform_drv);
    class_destroy(gpio_drv_data.class_gpio);
    pr_info("Module exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

// /* Create platform driver include init and exit */
// module_platform_driver(Module_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("007");
MODULE_DESCRIPTION("GPIO module driver for platform devices");
MODULE_INFO(boarder, "GPIO Platform Devices"); 