#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */
#include <linux/pagemap.h>   /* PAGE_CACHE_SIZE */
#include <linux/fs.h>        /* libfs stuff *//*the most important library, dentry inode file belongs to here*/
#include <asm/atomic.h>      /* atomic_t stuff */
#include <asm/uaccess.h>     /* copy_to_user */
#include <linux/slab.h>		 /*for allocating memory with kmallo(size,flag);*/

#define NAME "assoofs"

#define LFS_MAGIC 0x19980122 	//Magic number
#define TMPSIZE 100				//size of the text buffer.

#define DEF_PER_FILE 0644	
#define DEF_PER_DIR 0755
#define OK 0					

/********************************************************************************************************************************************************
 * IMPORTANTE, ESTE FLAG INDICA EL MODO DE SISTEMA QUE SE EJECUTARA, SI ESTA A 0 FUNCIONARA COMO FICHEROS CON TEXTO, SI ESTA A 1 COMO FICHEROS CONTADOR *
 ********************************************************************************************************************************************************/
#define COUNT_MODE 1
/**************************************************************************************************************************************************
 * IMPORTANT, THIS FLAG DETERMINES THE MODE OF THE SYSTEM, IF THE VALUE IS 0 IT WILL BE A TEXT VFS, OTHERWISE WILL BE A  VFS COMPOSED BY COUNTERS *
 **************************************************************************************************************************************************/


 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Manuel Fidalgo Fierro");

/*Counters for the two default files*/
 static atomic_t counter_1, counter_2;


 /*HEADERS*/
 static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data);
 static int assoofs_fill_super(struct super_block *sb, void * data, int silent);

 static int assoofs_create_files(struct super_block *sb, struct dentry * root);
 static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter);
 static struct dentry  * assoofs_create_directory(struct super_block *sb, struct dentry *dir, const char * name);

 static struct inode * assoofs_make_inode(struct super_block * sb, int mode);

 static int assoofs_open(struct inode * inode, struct file * flip);
 static int assoofs_read_file(struct file * flip, char * buf, size_t count, loff_t * offset);
 static ssize_t assoofs_write_file(struct file * flip, const char * buf, size_t count,loff_t * offset);

 static struct dentry * assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags);
 static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);
 static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);


 /*Defines the system type, name and the funcions when the systmen is mounted and unmounted*/
 static struct file_system_type assoofs_type = {

 	.owner = THIS_MODULE,
 	.name = NAME,
	.mount = assoofs_get_super, //Funcion para montar el sistema
	.kill_sb = kill_litter_super, 
	
};

/*task soported by the system*/
static struct super_operations assoofs_s_ops = {

	.statfs = simple_statfs,
	.drop_inode = generic_delete_inode,

};

/*File operations*/
static struct file_operations assoofs_file_ops = {

	.open = assoofs_open,
	.read = assoofs_read_file,
	.write = assoofs_write_file,

};

/*Inode operations*/
static struct inode_operations assoofs_inode_ops = {

	.create = assoofs_create,
	.lookup = assoofs_lookup,
	.mkdir = assoofs_mkdir,

};


/*Funcion that is called when the superblock is created*/
static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data){
	return mount_bdev(fst,flags,devname,data,assoofs_fill_super);
}

/*fill the superblock param the superblock to fill*/
static int assoofs_fill_super(struct super_block *sb, void * data, int silent){

	struct dentry *root_dentry;
	struct inode * root_inode;
	struct qstr qname;

	qname.name = "root";
	qname.len = 4;
	qname.hash = full_name_hash("root", 4);

	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = LFS_MAGIC; //Magic number
	sb->s_op = &assoofs_s_ops; //Super_operations
	
	root_inode = assoofs_make_inode(sb,S_IFDIR | DEF_PER_DIR );

	root_inode->i_op = &assoofs_inode_ops; 	//Deffault opperations, allows create new files an directories
	root_inode->i_fop = &simple_dir_operations; 

	root_dentry = d_make_root(root_inode);
	
	sb->s_root = root_dentry;

	assoofs_create_files(sb,root_dentry);


	return OK;
}

/*Creates all the default files in this VFS*/
static int assoofs_create_files(struct super_block *sb, struct dentry * root){
	struct dentry * dir;

	atomic_set(&counter_1,0); //init the first counter
	assoofs_create_file(sb,root,"counter_1",&counter_1); // A different counter for each default files

	dir = assoofs_create_directory(sb,root,"directory_0");

	atomic_set(&counter_2,0);
	assoofs_create_file(sb,dir,"counter_2",&counter_2); //dir as the parent directory, this file will be created in the file

	return OK;
}

/*creates a file and return his directory entry*/
static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter){
	struct dentry * dentry;
	struct inode * inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen(name);
	qname.hash = full_name_hash(name, qname.len);

	dentry = d_alloc(dir, &qname);

	inode = assoofs_make_inode(sb, S_IFREG | DEF_PER_FILE );//Flag with de default permissions 
	
	inode->i_fop = &assoofs_file_ops; //file operations
	if(COUNT_MODE){
		inode->i_private = counter; //Counter
	}else{
		inode->i_private = (char *)kmalloc(TMPSIZE*sizeof(char),GFP_KERNEL); //A memory space for each file
		memset(inode->i_private,0,TMPSIZE*sizeof(char));					 //Inits this memory block
	}
	
	
	d_add(dentry,inode); //associates dentry and inode
	
	return *dentry; //return the dentry
}
/*look up the file*/
static struct dentry * assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags){

	printk(KERN_INFO "assoofs_lookup\n");
	return NULL;
}
/*creates a directory, dentry is inicialized*/
static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode){

	struct inode * inode = assoofs_make_inode(dir->i_sb, S_IFDIR | DEF_PER_DIR);

	inode->i_op = &assoofs_inode_ops;
	inode->i_fop = &simple_dir_operations;

	d_add(dentry,inode);

	printk(KERN_INFO "assoofs_mkdir sucess\n");
	return OK;
}

atomic_t cout; 	/*Contador que van a usar todos los nuevos ficheros creados*/
/*Creates a new file with a global conunter(the same counter for al the new files)*/
static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl){

	struct inode * inode;

	atomic_set(&cout,0);
	inode = assoofs_make_inode(dir->i_sb,mode);

	inode->i_fop = &assoofs_file_ops; //Operations suported

	if(COUNT_MODE){
		inode->i_private = &cout; //global counter for al the new files
	}else{
		inode->i_private = (char *)kmalloc(TMPSIZE*sizeof(char),GFP_KERNEL); //memory segment for each file
		memset(inode->i_private,0,TMPSIZE*sizeof(char));
	}

	d_add(dentry,inode);

	printk(KERN_INFO "assoofs_create sucess\n");
	return OK;
}

/*create a directory, struct dentry is inicialized*/
static struct dentry * assoofs_create_directory(struct super_block *sb, struct dentry *dir, const char * name){

	struct dentry * dentry;
	struct inode * inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen(name);
	qname.hash = full_name_hash(name, qname.len);

	dentry = d_alloc(dir, &qname);

	inode = assoofs_make_inode(sb, S_IFDIR | DEF_PER_DIR);

	inode->i_op = &assoofs_inode_ops;
	inode->i_fop = &simple_dir_operations; 

	d_add(dentry,inode);
	
	return dentry;
}

/*Create and inicialice the inode fields*/
static struct inode * assoofs_make_inode(struct super_block * sb, int mode){
	struct inode * ret;
	ret = new_inode(sb);
	if(ret){
		ret-> i_mode = mode;
		ret->i_uid.val = ret->i_gid.val = 0;
		ret-> i_blocks = 0;
		ret-> i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
	}
	return ret;
}

/*Assing the inode i_private(counter or text) int the file->private_data field*/
static int assoofs_open(struct inode * inode, struct file *flip){
	flip->private_data = inode->i_private;
	return 0;
}

/*Read a file params: the buffer in user space, counter of bytes and teh offset pointer*/
static int assoofs_read_file(struct file * flip, char * buf, size_t count, loff_t * offset){

	if(COUNT_MODE){
		atomic_t * counter;	
		int len, v;	
		char tmp[TMPSIZE];	
		
		counter = (atomic_t *) flip->private_data;
		v = atomic_read(counter);
		
		if(*offset>0){
			printk(KERN_INFO "offset>0, v=-1\n");
			v-= 1; //value is offset is > 0
		}else{
			atomic_inc(counter);
		}

		len = snprintf(tmp,TMPSIZE,"%d\n",v); //transform the number in string an return the amount of bytes written in the buffer

		if(*offset > len){
			printk(KERN_INFO "*offset > len, returned OK\n");
			return OK;
		}
		if(count> len -* offset){
			printk(KERN_INFO "count> len -* offset, count = len - *offset\n");
			count = len - *offset;
		}
		/*Copies to buff(user space) the content in tmp(Kernel space), the amount of bytes indicantes by the last parameter*/
		if(copy_to_user(buf,tmp + *offset, count))
			return -EFAULT;		
		*offset += count;
		return count;

	}else{

		int i;
		char tmp[TMPSIZE];
		char * pt;
		int len = TMPSIZE; //maximun size of the buffer
		
		if(*offset > len){
			printk(KERN_INFO "*offset > len, returned OK\n");
			return OK;
		}
		if(count > len -* offset){
			printk(KERN_INFO "count> len -* offset, count = len - *offset\n");
			count = len - *offset;
		}
		pt = flip->private_data;
		printk(KERN_INFO "llamda a fileread\n");
		for(i=0; i< TMPSIZE; i++){
			tmp[i] = pt[i];
		}
		printk(KERN_INFO "Menssage en tmp -> %s",tmp);
		copy_to_user(buf,tmp+*offset,count);
		*offset+=count;
		return count;
	}

}

static ssize_t assoofs_write_file(struct file * flip, const char * buf, size_t count ,loff_t * offset){
	
	if(COUNT_MODE){
		atomic_t * counter;
		char tmp[TMPSIZE];
		//printk(KERN_INFO "assoofs_write_file buf->%s, cout->%d offset->%d\n",buf,count,(int)*offset); //just for debugg
		counter = (atomic_t *) flip->private_data;
		if(*offset!=0){
			printk(KERN_INFO "offset!=0, returned -EINVAL\n");
			return -EINVAL;
		}
		if(count >= TMPSIZE){
			printk(KERN_INFO "count >= TMPSIZE, returned -EINVAL\n");
			return -EINVAL;
		}
		memset(tmp,0,TMPSIZE); //fills tmo array with 0
		if(copy_from_user(tmp,buf,count)) //copy to tmp(kernel space) the content in buf(uses space) 
			return -EFAULT;
		/*set the counter with the integer value transformed from tmp in base 10, simple_strtol is a deprecated funcion*/
		atomic_set(counter, simple_strtol(tmp,NULL,10));
		return count;
	}else{

		int i;
		int acum=0;
		char tmp[TMPSIZE];
		char * pt;

		pt = flip->private_data;
		memset(tmp,0,TMPSIZE);
		printk(KERN_INFO "tmp->%s",tmp);
		copy_from_user(tmp,buf,count);
		for(i=0; i<TMPSIZE;i++){
			acum++;
			if(tmp[i]=='\0'){	//This copies the string until the endstring character, if the string length is > TMPSIZE could produce a bug
				break;
			}
		}
		printk(KERN_INFO "tmp->%s, size->%d",tmp,acum);
		for(i=0; i<TMPSIZE;i++){
			pt[i] = tmp[i];
		}
		return count;
	}
}


static int __init assoofs_init(void){

	printk(KERN_INFO "insertado modulo asssofs \n");
    return register_filesystem(&assoofs_type); /*file system type memory adress*/
}

static void __exit cleanup_assoofs(void){

	printk(KERN_INFO "extraido modulo asssofs \n");

}


module_init(assoofs_init);
module_exit(cleanup_assoofs);