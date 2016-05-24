#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */
#include <linux/pagemap.h>   /* PAGE_CACHE_SIZE */
#include <linux/fs.h>        /* libfs stuff *//*Lpara configurar el superbloque*/
#include <asm/atomic.h>      /* atomic_t stuff */
#include <asm/uaccess.h>     /* copy_to_user */

#define NAME "assoofs"
#define DEF_PER_FILE 0644
#define DEF_PER_DIR

/**
funciones que empieizen pir assofs hay que implenetarlo nosotros
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel Fidalgo Fierro");

/**
 Hay que defnir un superbloque inicializarlo y configurarlo
*/
 /*HEADERS*/
 static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data);
 static int assoofs_fill_super(struct super_block *sb, void * data, int silent);

 static void assoofs_create_files(struct super_block *sb, struct dentry * root);
 static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter);
 static struct dentry  assoofs_create_directory(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter);

 static struct inode * assoofsmakeinode(struct superblock * sb, int mode);

 static int assoofs_open(struct inode * inode, struct file *flip);
 static int assoofs_read_file(struct file * flip, char * buf, size_t count, loff_t * offset);
 static ssize_t assoofs_write_file(struct file * flip, const char * buf, size_t count,loff_t * offset);
/*/HEADERS*/

/*STRUCTS*/
 /*Define el typo de sistema que hemos creado, tipo nombre y funciones que se llaman al montar y desmontar*/
static struct file_system_type assoofs_type = {

	.owner = THIS_MODULE,
	.name = "assoofs",
	.mount = assoofs_get_super, //fuinioc que se llama al monstar
	.kill_sb = kill_litter_super, //fucnion que se llama al demontar
	
};

/*Estructura de las tareas que soporta el sistema*/
static struct super_operations assoofs_s_ops = {

	.statfs = simple_statfs,
	.drop_inode = generic_delete_inode,

};

/*Struct que define las operaciones sobre un fichero*/
static struct file_operations assoofs_file_ops = {

	.open = assoofs_open,
	.read = assofs_read_file,
	.write = assoofs_write_file,

};



/*Funcion que se llama al crear el superbloque*/
static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data){
	return mount_bdev(fts,flags,devname,data,assoofs_fill_super);
}

/*Llena el superbloque que se le pasa como puntero super_block*/
static int assoofs_fill_super(struct super_block *sb, void * data, int silent){

	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = LFS_MAGIC; //Numero magico
	sb->s_op = &assoofs_s_ops; //definimos la estructura con todas las operaciones que soporte el sistema de ficheros
	/*crear el root haciendo que el campo root del superbloque*/
	//lalamas a la funcion create files pasandole el puntero raiz y el superbloque
}

/*Creara los ficheros y las carpetas que se crean en el programa*/
static void assoofs_create_files(struct super_block *sb, struct dentry * root){
	static atomic_t counter;
	/**/
	atomic_set(&counter,0); //inicializa el contador a cero
	assoofs_create_file(sb,root,"counter",&counter); // crear un contador para cada fichero por separado;
	/**/
}

/*Crea un solo fichero, toma el superblo y se usa una estructura dentry(Entrad al directorio o algo asi) del dorecori cdonce lo queramos crear*/
static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter){
	struct dentry * dentry;
	struct inode * inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen(name);
	qname.hash = full_name_hash(name, qname.len);

	dentry = d_alloc(dir, &qname);

	//if(!dentry) goto out;
	inode = assofs_make_inode(sb, S_IFREG | DEF_PER_FILE );//flag que indica el tipo y los permisos por defecto, 
	//if(!inode) goto out_dput;
	inode->i_fop = &assofs_file_ops; //Operaciones que va a soportar
	inode-> i_private = counter; // campo donde se le asgina el contador, solo apra kernel 3.0 o superior cuidado con la informacion
	d_add(dentry,inode); //añadimos la estructura dentry y el dentry,ufs capa intermedia que abstrae(clase abstracta que luego referecnia a una clase real)
	return dentry; //devolvemos el struct dentry;
}

/*Para la creacion de directorios*/
static struct dentry assoofs_create_directory(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter){

}

/*funcion ara crear inodo, declaramos el nuevo inode e iniciamlizamos los valores*/
static struct inode * assoofsmakeinode(struct superblock * sb, int mode){
	
	struct inode * ret; 
	ret = new_inode(sb);

	if(ret){
		ret−>imode = mode;
		ret−>iuid.val = ret−>igid.val = 0;
		ret−>iblocks = 0;
		ret−>i_atime = ret−>i_mtime = ret−>i_ctime = CURRENT_TIME;
	}
	return ret;
}

static int assoofs_open(struct inode * inode, struct file *flip){
	flip->private_data = inode->i_private;
	return 0;
}

/*letura de un fichero*/
static int assoofs_read_file(struct file * flip, char * buf, size_t count, loff_t * offset){
	atomic_t v;
	int len;
	v = atomic_read(counter);
	if(*offset>0) 
		v-= 1;
	else
		atomic_inc(counter);

	len = snprintf(tmp,TMPSIZE,"%d\n",v);

	if(*offset > len)
		return 0;
	
	if(count>len-*offset)
		count = len - *offset;
	
	if(copy_to_user(buf,tmp + *offset, count))
		return -EFAULT;

	*offset += count;
	return count;
}

/*escritura de un fichero*/
static ssize_t assoofs_write_file(struct file * flip, const char * buf, size_t count,loff_t * offset){
	atomic_t * counter;

	counter = (atomic_t *) flip->private_data;
	/* */
	memset(tmp,0,TMPSIZE);
	if(copy_from_user(tmp,buf,count))
		return -EFAULT;
	atomic_set(counter, simple_strol(tmp,NULL,10));
	return count;
}

static int __init assoofs_init(void){
	
	printk(KERN_INFO "Init \n");
    return register_filesystem(&assoofs_type); //toma como argumento la direcion de memroria de una estructura

}

static void __exit cleanup_hello(void){

	printk(KERN_INFO "End\n");

}

module_init(init_hello);
module_exit(cleanup_hello);