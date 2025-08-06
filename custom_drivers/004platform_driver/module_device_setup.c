#include<linux/module.h>
#include<linux/platform_device.h>
#include"platform.h"

/*  1. Create platform data*/
struct Module_platform_data module_platform_data[2] = {
    [0] = {
        .size =512, .perm = RDWR, .serial_number = "001"
    },
    [1] = {
        .size =1024, .perm = RDWR, .serial_number = "002"
    }
};

/*  2. Create 2 platform devices*/
struct platform_device platform_device1 ={
    .name = "char_device",
    .id = 1,
    .dev = {
        .platform_data = &module_platform_data[0], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};

struct platform_device platform_device2 ={
    .name = "char_device",
    .id = 2,
    .dev = {
        .platform_data = &module_platform_data[1], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};

void module_platform_release(struct device *dev){
    pr_info(KERN_INFO "%s: Platform device released\n", __func__);
    // Additional cleanup can be done here if necessary
}

static int __init module_platform_device_init(void){
    pr_info(KERN_INFO "%s: Initializing platform devices\n", __func__);
    platform_device_register(&platform_device1);
    platform_device_register(&platform_device2);
    return 0;
}

static void __exit module_platform_device_exit(void){
    pr_info(KERN_INFO "%s: Exiting platform devices\n", __func__);
    platform_device_unregister(&platform_device1);
    platform_device_unregister(&platform_device2);
}

module_init(module_platform_device_init);
module_exit(module_platform_device_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("004");
MODULE_DESCRIPTION("A simple module driver for a platform device");
MODULE_INFO(boarder, "Module");
