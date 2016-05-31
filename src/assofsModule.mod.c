#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x141d70f7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7fbf7f2d, __VMLINUX_SYMBOL_STR(kill_litter_super) },
	{ 0xe49b0af0, __VMLINUX_SYMBOL_STR(simple_statfs) },
	{ 0xadc42932, __VMLINUX_SYMBOL_STR(generic_delete_inode) },
	{ 0x3b18588e, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0xc94eb4b1, __VMLINUX_SYMBOL_STR(d_make_root) },
	{ 0x1896ba7e, __VMLINUX_SYMBOL_STR(simple_dir_operations) },
	{ 0xe995f48c, __VMLINUX_SYMBOL_STR(simple_dir_inode_operations) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x40fdccee, __VMLINUX_SYMBOL_STR(d_rehash) },
	{ 0x24e54cff, __VMLINUX_SYMBOL_STR(d_instantiate) },
	{ 0x1890dc64, __VMLINUX_SYMBOL_STR(d_alloc) },
	{ 0x6f20960a, __VMLINUX_SYMBOL_STR(full_name_hash) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xb742fd7, __VMLINUX_SYMBOL_STR(simple_strtol) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x34184afe, __VMLINUX_SYMBOL_STR(current_kernel_time) },
	{ 0x707baa85, __VMLINUX_SYMBOL_STR(new_inode) },
	{ 0x4ede024, __VMLINUX_SYMBOL_STR(mount_bdev) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5EFE6D09184B0AA85A1DF65");
