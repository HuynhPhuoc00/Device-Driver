#include"spi-sysfs.h"

struct drv_private_data drv_data;

static ssize_t text_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count)
{
    return count;
}

static DEVICE_ATTR_WO(text);

static const struct of_device_id dev_of_match[] = {
    {},
    {}
};
MODULE_DEVICE_TABLE(of, dev_of_match);

static struct spi_driver dev_driver = {
    .driver = {
        .name = DEV_NAME,
        .of_match_table = dev_of_match,
    },
    .probe = spi_sysfs_probe,
    .remove = spi_sysfs_remove,
};


/* Get and put lcd */
int spi_sysfs_probe(struct spi_device *spi){
    // lcd_drv_data.client = client;
    // Create class and device
    drv_data.dev_class = class_create(THIS_MODULE, DEV_NAME);

    if (IS_ERR(drv_data.dev_device)){
        pr_err("Failed to create class\n");
        return PTR_ERR(drv_data.dev_class);
    }

    drv_data.dev_device = device_create(drv_data.dev_class,
                                            &spi->dev,
                                            0,
                                            NULL,
                                            DEV_NAME);
    device_create_file(drv_data.dev_device, &dev_attr_text);
    if (IS_ERR(drv_data.dev_device)) {
        pr_err("Failed to create device\n");
        class_destroy(drv_data.dev_class);
        return PTR_ERR(drv_data.dev_device);
    }

    pr_info("lcd probe ok\n");
    return 0;
}

int spi_sysfs_remove(struct spi_device *spi){
    device_remove_file(drv_data.dev_device, &dev_attr_text);
    device_destroy(drv_data.dev_class, 0);
    class_destroy(drv_data.dev_class);
    return 0;
}


/* Init and Exit module*/
static int __init __my_module_driver_init(void){
    pr_info("Module init\n");
    return spi_register_driver(&dev_driver);
}

static void __exit __my_module_driver_exit(void){
    spi_unregister_driver(&dev_driver);
    pr_info("Module exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

// /* Create platform driver include init and exit */
// module_platform_driver(Module_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LCD");
MODULE_DESCRIPTION("LCD module driver");