#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE( "GPL" );

static int globalVar = 0;

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
        printk( "in globalVar_ioctl: bad cmd\n" );
    }
    return 0;
}

static int globalVar_open( struct inode* inodp, struct file* filp ){
    printk( "in globalVar_open func:\n" );
    if( !try_module_get(THIS_MODULE) ){
        printk( "Could not reserve module\n" );
        return -1;
    }
    return 0;
}

static int globalVar_release( struct inode* inodp, struct file* filp ){
    printk( "relese globalvar\n" );
    module_put( THIS_MODULE );
    return 0;
}

static ssize_t globalVar_read( struct file* filp, char* __user buf, size_t len, loff_t* offset ){
    if( copy_to_user(buf, &globalVar, sizeof(int)) ){
        return -EFAULT;
    }
    return sizeof( int );
}

static ssize_t globalVar_write( struct file* filp, const char* __user buf, size_t len, loff_t* offset ){
    if( copy_from_user(&globalVar, buf, sizeof(int)) ){
        return - EFAULT;
    }
    return sizeof( int );
}

static int __init globalVar_init( void ){
    int ret;
    
    ret = 0;
    
    return ret;
}
module_init( globalVar_init );

static void __exit globalVar_exit( void ){
    
}
module_exit( globalVar_exit );

