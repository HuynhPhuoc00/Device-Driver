#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0xc8dcc62a, "krealloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x67a33343, "devm_kmalloc" },
	{ 0x81ef79db, "platform_driver_unregister" },
	{ 0x1fc34ff, "class_destroy" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x3b4cf56c, "_dev_info" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xaabf546, "cdev_add" },
	{ 0x3a1b27aa, "_dev_err" },
	{ 0xf4ee463a, "device_create" },
	{ 0xc74fa6a1, "class_create" },
	{ 0x6059ecc8, "sysfs_create_group" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xfc802b99, "__platform_driver_register" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x3c06c15f, "device_destroy" },
	{ 0x3854774b, "kstrtoll" },
	{ 0x3185476b, "cdev_init" },
	{ 0xf41c271a, "cdev_del" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "796213AB6BB67A88BB1C5B3");
