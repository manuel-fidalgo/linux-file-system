#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */
#include <linux/pagemap.h>   /* PAGE_CACHE_SIZE */
#include <linux/fs.h>        /* libfs stuff *//*Lpara configurar el superbloque*/
#include <asm/atomic.h>      /* atomic_t stuff */
#include <asm/uaccess.h>     /* copy_to_user */

#define NAME "assoofs"

#define LFS_MAGIC 0x19980122 //Necesary or include in the library ?
#define TMPSIZE 20

#define DEF_PER_FILE 0644
#define DEF_PER_DIR 0755

#define OK 0

/**
funciones que empieizen pir assofs hay que implenetarlo nosotros
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel Fidalgo Fierro");
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

 /*Para la parte opcional*/
 static struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags);
 static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);
 static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);





 /*Define el typo de sistema que hemos creado, tipo nombre y funciones que se llaman al montar y desmontar*/
 static struct file_system_type assoofs_type = {

 	.owner = THIS_MODULE,
 	.name = NAME,
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
	.read = assoofs_read_file,
	.write = assoofs_write_file,

};
/*Struct para las operaciones con inodos*/
static struct inode_operations assoofs_inode_ops = {

	.create = assoofs_create,
	.lookup = assoofs_lookup,
	.mkdir = assoofs_mkdir,

};




/*Funcion que se llama al crear el superbloque*/
static struct dentry * assoofs_get_super(struct file_system_type * fst, int flags, const char *devname, void *data){
	return mount_bdev(fst,flags,devname,data,assoofs_fill_super);
}

/*Llena el superbloque que se le pasa como puntero super_block*/
static int assoofs_fill_super(struct super_block *sb, void * data, int silent){

	struct dentry *root_dentry;
	struct inode * root_inode;
	struct qstr qname;

	qname.name = "root";
	qname.len = 4;
	qname.hash = full_name_hash("root", 4);

	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = LFS_MAGIC; //Numero magico
	sb->s_op = &assoofs_s_ops; //definimos la estructura con todas las operaciones que soporte el sistema de ficheros/*crear el root haciendo que el campo root del superbloque*///lalamas a la funcion create files pasandole el puntero raiz y el superbloque
	
	root_inode = assoofs_make_inode(sb,S_IFDIR | DEF_PER_DIR );

	root_inode->i_op = &assoofs_inode_ops; 	//Cmabiamos las operaciones por defecto del inodo para las partes opcionales
	root_inode->i_fop = &simple_dir_operations;

	root_dentry = d_make_root(root_inode);
	
	sb->s_root = root_dentry;

	assoofs_create_files(sb,root_dentry);

		
	return OK;
}

/*Creara los ficheros y las carpetas que se crean en el programa*/
static int assoofs_create_files(struct super_block *sb, struct dentry * root){
	struct dentry * dir;

	atomic_set(&counter_1,0); //inicializa el contador a cero
	assoofs_create_file(sb,root,"counter_1",&counter_1); // crear un contador para cada fichero por separado

	dir = assoofs_create_directory(sb,root,"directory_0");

	atomic_set(&counter_2,0); //inicializa el contador a cero
	assoofs_create_file(sb,dir,"counter_2",&counter_2);/*le pasamos como root del direotorio que hemos creado*/


	return OK;
}

/*Crea un fichero y devuelve la entrada del direcorio del mismo*/
static struct dentry  assoofs_create_file(struct super_block *sb, struct dentry *dir, const char * name, atomic_t * counter){
	struct dentry * dentry;
	struct inode * inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen(name);
	qname.hash = full_name_hash(name, qname.len);

	dentry = d_alloc(dir, &qname);

	inode = assoofs_make_inode(sb, S_IFREG | DEF_PER_FILE );//flag que indica el tipo y los permisos por defecto, 
	
	inode->i_fop = &assoofs_file_ops; //Operaciones que va a soportar
	inode->i_private = counter; //campo donde se le asgina el contador, solo apra kernel 3.0 o superior cuidado con la informacion
	
	d_add(dentry,inode); //añadimos la estructura dentry y el dentry,ufs capa intermedia que abstrae(clase abstracta que luego referecnia a una clase real)
	
	return *dentry; //devolvemos el struct dentry;
}
/*
La implementación de las funciones assoofs_mkdir y assoofs_create es muy similar a la de las funciones assoofs_create_file y assoofs_create_dir.
Sólo que en este caso, solamente tenemos que crear el nuevo inodo y asignarle el contador. 
La estructura dentry que necesitamos nos viene como argumento en la función y ya está inicializada.
*/
static struct dentry * assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags){
	
	printk(KERN_INFO "assoofs_lookup\n");
	return NULL;
}
static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode){
	
	struct inode * inode = assoofs_make_inode(dir->i_sb, S_IFDIR | DEF_PER_DIR);

	inode->i_op = &assoofs_inode_ops;
	inode->i_fop = &simple_dir_operations;
	//inode->i_fop = &assoofs_inode_ops;
	d_add(dentry,inode);

	printk(KERN_INFO "assoofs_mkdir sucess\n");
	return OK;
}
static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl){

	atomic_t cout;
	atomic_set(&cout,0);

	struct inode * inode= assoofs_make_inode(dir->i_sb,mode);
	
	inode->i_fop = &assoofs_file_ops; //Operaciones que va a soportar
	inode->i_private = &cout; //habria que añadirle el contador, que ni idea de donde lo voy a sacar...

	d_add(dentry,inode);

	printk(KERN_INFO "assoofs_create sucess\n");
	return OK;
}

/*Para la creacion de directorios*/
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


/*Funcion ara crear inodo, declaramos el nuevo inode e iniciamlizamos los valores*/
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

/*Agrega el valor del campo i_private del inodo al campo private_data del fchero*/
static int assoofs_open(struct inode * inode, struct file *flip){
	flip->private_data = inode->i_private;
	return 0;
}

/*Letura de un fichero*//*se ejecuta como bucle si no hay condicion de parada*/
static int assoofs_read_file(struct file * flip, char * buf, size_t count, loff_t * offset){

	atomic_t * counter;			//Contador
	int len, v;					
	char tmp[TMPSIZE];
	len=0; v=0;

	counter = (atomic_t *) flip->private_data;
	v = atomic_read(counter);
	
	if(*offset>0){
		v-= 1; /*valor que se devolvera si el offset es cero*/
	}else{
		atomic_inc(counter);
	}

	len = snprintf(tmp,TMPSIZE,"%d\n",v); //convierte el numero a cadena
	if(*offset > len)
		return OK;

	if(count> len -* offset)
		count = len - *offset;
	
	if(copy_to_user(buf,tmp + *offset, count)) //copia a buf lo que hay en tmp + offset, tantos bytes como el parametro count indique. tmp en espacio de kernel y buf en espacio de usuario
		return -EFAULT;
	
	*offset += count;
	return count;
}

static ssize_t assoofs_write_file(struct file * flip, const char * buf, size_t count,loff_t * offset){

	atomic_t * counter;
	char tmp[TMPSIZE];

	counter = (atomic_t *) flip->private_data;


	if(*offset!=0)
		return -EINVAL;
	if(count >= TMPSIZE)
		return -EINVAL;
	
	memset(tmp,0,TMPSIZE);

	if(copy_from_user(tmp,buf,count)) //En tmp vamos a tener lo que el usuario le pase como codigo
		return -EFAULT;
	atomic_set(counter, simple_strtol(tmp,NULL,10));//Convierte la string en un unsigned long y se la pasa al contador, funcion obsoleta(puntero de la cadena inicial, puntero al final de la cadena, base para la conversion)
	return count;
}

static int __init assoofs_init(void){

	printk(KERN_INFO "insertado modulo asssofs \n");
    return register_filesystem(&assoofs_type); /*Direcion de memoria de una estructura*/
}

static void __exit cleanup_assoofs(void){

	printk(KERN_INFO "extraido modulo asssofs \n");

}


module_init(assoofs_init);
module_exit(cleanup_assoofs);
