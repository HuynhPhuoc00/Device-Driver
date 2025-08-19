#include"lcd-sysfs.h"

struct lcd_drv_private_data lcd_drv_data;

static ssize_t text_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t count)
{
    i2c_lcd_clear();
    i2c_send_string(buf);
    return count;
}

static DEVICE_ATTR_WO(text);

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


/* Get and put lcd */
int lcd_sysfs_probe(struct i2c_client *client, const struct i2c_device_id *id){
    // lcd_drv_data.client = client;
    // Create class and device
    lcd_drv_data.lcd_class = class_create(THIS_MODULE, LCD1602_NAME);

    if (IS_ERR(lcd_drv_data.lcd_class)){
        pr_err("Failed to create class\n");
        return PTR_ERR(lcd_drv_data.lcd_class);
    }

    lcd_drv_data.lcd_device = device_create(lcd_drv_data.lcd_class,
                                            &client->dev,
                                            0,
                                            NULL,
                                            LCD1602_NAME);
    device_create_file(lcd_drv_data.lcd_device, &dev_attr_text);
    if (IS_ERR(lcd_drv_data.lcd_device)) {
        pr_err("Failed to create device\n");
        class_destroy(lcd_drv_data.lcd_class);
        return PTR_ERR(lcd_drv_data.lcd_device);
    }
    
    i2c_lcd_init(client);
    pr_info("lcd probe ok\n");
    return 0;
}

int lcd_sysfs_remove(struct i2c_client *client){
    device_remove_file(lcd_drv_data.lcd_device, &dev_attr_text);
    device_destroy(lcd_drv_data.lcd_class, 0);
    class_destroy(lcd_drv_data.lcd_class);
    return 0;
}


/* Init and Exit module*/
static int __init __my_module_driver_init(void){
    pr_info("Module init\n");
    return i2c_add_driver(&lcd_driver);
}

static void __exit __my_module_driver_exit(void){
    i2c_lcd_clear();
    i2c_del_driver(&lcd_driver);
    pr_info("Module exit\n");
}

module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

// /* Create platform driver include init and exit */
// module_platform_driver(Module_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LCD");
MODULE_DESCRIPTION("LCD module driver");