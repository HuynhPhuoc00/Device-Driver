#include<linux/module.h>

static int __init hello_world_init(void){
    printk("Hello, World!\n");
    return 0;
}

static void __exit hello_world_exit(void){
    printk("Goodbye, World!\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("001");
MODULE_DESCRIPTION("A simple Hello World kernel module");
MODULE_INFO(boarder, "Hello World Module");
