#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x3b12617d, "module_layout" },
	{ 0x29e037f9, "device_remove_file" },
	{ 0xf6803648, "cdev_del" },
	{ 0x818f4cbd, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0xaf5d72f5, "i2c_del_driver" },
	{ 0x504b9a95, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc5850110, "printk" },
	{ 0x2b73afef, "device_create" },
	{ 0x2dea4a30, "_dev_err" },
	{ 0xd3b80a4a, "i2c_register_driver" },
	{ 0x23836833, "device_create_file" },
	{ 0x7cf5b970, "cdev_add" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0x482bc9de, "i2c_transfer_buffer_flags" },
	{ 0x314b20c8, "scnprintf" },
	{ 0x3b621568, "class_destroy" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x5e131dd8, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Corg,_sht3x");
MODULE_ALIAS("of:N*T*Corg,_sht3xC*");
MODULE_ALIAS("i2c:sht30");
