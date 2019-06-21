#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE( "GPL" );

int  catFlag;
char gBuf[ 100 ];
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
        printk( "in globalVar_ioctl: bad cmd!\n" );
    }
    return 0;
}

static int globalVar_open( struct inode* inodp, struct file* filp ){
    printk( "in globalVar_open func: get this module\n" );
    if( !try_module_get(THIS_MODULE) ){
        printk( "Could not reserve module\n" );
        return -1;
    }

    catFlag = 0;

    return 0;
}

static int globalVar_release( struct inode* inodp, struct file* filp ){
    printk( "in globalVar_release func: put this module\n" );
    module_put( THIS_MODULE );
    
    catFlag = 0;

    return 0;
}

static ssize_t globalVar_read( struct file* filp, char* __user buf, size_t len, loff_t* offset ){

	//char buf[200];

	size_t tmp;

    printk( "call globalVar_read func, len = %lu\n", len );
    printk( "private data:%s\n", (char*)filp->private_data );

#if 0
    if( catFlag ){
    	//copy_to_user( buf, "-1", 1 );
        catFlag = 0;
        return 0;
    }

    len = strlen( gBuf );
    len++;
#endif

    len > 100 ? tmp = 100 : len;
    printk( "len = %lu\n", tmp );

    copy_to_user( buf, gBuf, tmp );

    catFlag = 1;
    return 20;
}

static ssize_t globalVar_write( struct file* filp, const char* __user buf, size_t len, loff_t* offset ){
    printk( "call globalVar_write func, len = %lu\n", len );
    
    memset( gBuf, 0, sizeof(gBuf) );
    copy_from_user( gBuf, buf, len );

    return 0;
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
    
    printk( "register chrdev success! cdevMajor is:%+d\n", cdevMajor );
    return 0;
}
module_init( globalVar_init );

static void __exit globalVar_exit( void ){
    unregister_chrdev( cdevMajor, "globalVar" );
    printk( "globalVar exit!\n" );
}
module_exit( globalVar_exit );

