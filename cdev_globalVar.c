#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );

extern int __init globalMem_bus_init( void );
extern void __exit globalMem_bus_exit( void );

unsigned int  offset_read;   /* 设备文件读偏移量 */
unsigned int  offset_write;  /* 设备文件写偏移量 */
char gBuf[ 100 ];            /* 设备文件内存缓冲区 */
unsigned int cdevMajor;

static ssize_t  globalVar_read( struct file*, char* __user, size_t, loff_t* );
static ssize_t  globalVar_write( struct file*, const char* __user, size_t, loff_t* );
static long     globalVar_ioctl( struct file*, unsigned int, unsigned long );
static int      globalVar_open( struct inode*, struct file* );
static int      globalVar_release( struct inode*, struct file* );

struct file_operations globalVar_fops = {
    owner:             THIS_MODULE,
    read:              globalVar_read,
    write:             globalVar_write,
    unlocked_ioctl:    globalVar_ioctl,
    open:              globalVar_open,
    release:           globalVar_release,
};

static long globalVar_ioctl( struct file* filp, unsigned int cmd, unsigned long arg ){
    printk( "in globalVar_ioctl func:\n" );
    switch( cmd ){
    case 1:
        printk( "in globalVar_ioctl: CMD NO: 1\n" );
        break;
    default:
        printk( "in globalVar_ioctl: bad cmd:%u\n", cmd );
    }
    return 0;
}

static int globalVar_open( struct inode* inodp, struct file* filp ){

    printk( "in globalVar_open func: get this module\n" );
    if( !try_module_get(THIS_MODULE) ){
        printk( "Could not reserve module\n" );
        return -1;
    }

    offset_read  = 0;
    offset_write = 0;

    return 0;
}

static int globalVar_release( struct inode* inodp, struct file* filp ){
    printk( "in globalVar_release func: put this module\n" );
    module_put( THIS_MODULE );

    return 0;
}

static ssize_t globalVar_read( struct file* filp, char* __user buf, size_t len, loff_t* offset ){

	size_t tmp;

    printk( "call globalVar_read func, len = %lu\n", len );
    printk( "read offset:%lli\n", *offset );
    printk( "file offset:%lli\n",  filp->f_pos );
    printk( "private data:%s\n", (char*)filp->private_data );

    tmp = strlen( gBuf );  /* strlen()函数会返回缓冲区中字符串的长度，不包含该字符串末尾的null结束符 */
    tmp++;
    if( offset_read >= tmp )
    	return 0;

    tmp = tmp - offset_read;
    copy_to_user( buf, gBuf, tmp );
    offset_read = tmp;

    return offset_read;
}

static ssize_t globalVar_write( struct file* filp, const char* __user buf, size_t len, loff_t* offset ){

	unsigned int tmp;

	printk( "call globalVar_write func, len = %lu\n", len );
    printk( "file offset is: %lli\n", *offset );
    printk( "write offset is: %u\n", offset_write );

	if( offset_write == 0 )
		memset( gBuf, 0, sizeof(gBuf) );

    tmp = (len>99) ? 99 : len;
    printk( "tmp is: %u\n", tmp );

    if( (offset_write + tmp) > 99 )
    	offset_write = 0;
    
    copy_from_user( (gBuf+offset_write), buf, tmp );
    offset_write += tmp;

    return len;
}

static int __init globalVar_init( void ){
    //int ret;
    //dev_t cdevNo;

/*    
    ret = alloc_chrdev_region( &cdevNo, 0, 1, "globalVar" );
    if( ret < 0 )
        return ret;
        
    cdevMajor = MAJOR( cdevNo );
    printk( "cdevNo is: %+d\n", cdevNo );
    printk( "cdevMajor is %d\n", cdevMajor );
    printk( "minor is %d\n", MINOR(cdevNo) );
*/
    memset( gBuf, 0, sizeof(gBuf) );
    strcpy( gBuf, "hello faye\n" );
    
    cdevMajor = register_chrdev( 0, "globalVar", &globalVar_fops );
    if( cdevMajor < 0 ){
        printk( "register chrdev failure! ret = %+d\n", cdevMajor );
        return cdevMajor;
    }
    
    globalMem_bus_init();
    
    printk( "register chrdev success! cdevMajor is:%+d\n", cdevMajor );
    return 0;
}
module_init( globalVar_init );

static void __exit globalVar_exit( void ){
    
    globalMem_bus_exit();
    
    unregister_chrdev( cdevMajor, "globalVar" );
    printk( "globalVar exit!\n" );
}
module_exit( globalVar_exit );

