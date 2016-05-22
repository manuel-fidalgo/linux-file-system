#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */
#include <linux/pagemap.h>   /* PAGE_CACHE_SIZE */
#include <linux/fs.h>        /* libfs stuff */
#include <asm/atomic.h>      /* atomic_t stuff */
#include <asm/uaccess.h>     /* copy_to_user */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel Fidalgo Fierro");

static struct file_system_type assoofs_type = {
	.owner = THIS_MODULE,
	.name = "assoofs",
	.mount = assoofs_get_super,
	.kill_sb = kill_litter_super,
	
};
static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data){
	return mount_bdeb(fts,flags,devname,data,assoofs_fill_super);
}
static int assoofs_fill_super(struct super_block *sb, void * data, int silent){
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = LFS_MAGIC;
	sb->s_op = &assoofs_s_ops;
}


static int __init assoofs_init(void){

    printk(KERN_INFO "Init \n");
    return register_filesystem(&assoofs_type);
}

static void __exit cleanup_hello(void){

    printk(KERN_INFO "End\n");
}

module_init(init_hello);
module_exit(cleanup_hello);