#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */
#include <linux/pagemap.h>   /* PAGE_CACHE_SIZE */
#include <linux/fs.h>        /* libfs stuff */
#include <asm/atomic.h>      /* atomic_t stuff */
#include <asm/uaccess.h>     /* copy_to_user */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel Fidalgo Fierro");

static atomic_t counter;

static struct file_system_type assoofs_type = {
	.owner = THIS_MODULE,
	.name = "assoofs",
	.mount = assoofs_get_super,
	.kill_sb = kill_litter_super,
	
};

static struct super_operations assoofs_s_ops = {
	.statfs = simple_statfs,
	.drop_inode = generic_delete_inode,
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

static void assoofs_create_files(struct super_block *sb, struct dentry * root){

	/**/
	atomic_set(&counter,0);
	assoofs_create_file(sb,root,"counter",&counter);
	/**/
}
static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter){
	struct dentry * dentry;
	struct inode * inode;
	struct qstr qname;
	qname.name = name;
	qname.len = strlen(name);
	qname.hash = full_name_hash(name, qname.len);
	dentry = d_alloc(dir, &qname);
	if(!dentry) //goto out;?
	inode = assofs_make_inode(sb,S_IFREG | 0644);
	if(!inode) goto out_dput;
	inode->i_fop = &assofs_file_ops;
	inode-> i_private = counter;
	d_add(dentry,inode);
	return dentry;
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