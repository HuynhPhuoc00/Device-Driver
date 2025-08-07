#include<linux/module.h>
#include<linux/platform_device.h>
#include<linux/mod_devicetable.h>
#include"platform.h"

/*  1. Create platform data*/
struct Module_platform_data module_platform_data[4] = {
    [0] = {
        .size =512, .perm = RDWR,   .serial_number = "001"
    },
    [1] = {
        .size =1024,.perm = RDWR,   .serial_number = "002"
    },
    [2] = {
        .size =128, .perm = RDONLY, .serial_number = "003"
    },
    [3] = {
        .size =32,  .perm = WRONLY, .serial_number = "004"
    },
};

/*  2. Create 2 platform devices*/
struct platform_device platform_device1 ={
    .name = "device_A",
    .id = 1,
    .dev = {
        .platform_data = &module_platform_data[0], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};

struct platform_device platform_device2 ={
    .name = "device_B",
    .id = 2,
    .dev = {
        .platform_data = &module_platform_data[1], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};

struct platform_device platform_device3 ={
    .name = "device_C",
    .id = 3,
    .dev = {
        .platform_data = &module_platform_data[2], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};

struct platform_device platform_device4 ={
    .name = "device_D",
    .id = 4,
    .dev = {
        .platform_data = &module_platform_data[3], // Assign platform data
        .release = module_platform_release, // Set release function
    }
};


struct platform_device *platform_dev[] = {
    &platform_device1,
    &platform_device2,
    &platform_device3,
    &platform_device4,
};

void module_platform_release(struct device *dev){
    pr_info("%s: Platform device released\n", __func__);
    // Additional cleanup can be done here if necessary
}

static int __init module_platform_device_init(void){
    pr_info("%s: Initializing platform devices\n", __func__);
    platform_add_devices(platform_dev, ARRAY_SIZE(platform_dev));
    return 0;
}


static void __exit module_platform_device_exit(void){
    pr_info("%s: Exiting platform devices\n", __func__);
    platform_device_unregister(&platform_device1);
    platform_device_unregister(&platform_device2);
    platform_device_unregister(&platform_device3);
    platform_device_unregister(&platform_device4);
}

module_init(module_platform_device_init);
module_exit(module_platform_device_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("004");
MODULE_DESCRIPTION("A simple module driver for a platform device");
MODULE_INFO(boarder, "Module");
