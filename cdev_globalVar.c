#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include "cdev_bus.h"
#include "cdev_device.h"
#include "cdev_driver.h"

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );  /*
    * 注意,本行不可省略,否则即使能成功编译,但在加载本模块时,
    * 会提示"Unknown symbol in module",并会在dmesg命令中给出所缺少的符号 */


unsigned int  offset_read;   /* 设备文件当前读偏移量 */
unsigned int  offset_write;  /* 设备文件当前写偏移量 */
char gBuf[ 100 ];            /* 本字符设备文件的内存缓冲区 */
unsigned int cdevMajor;      /* 本字符设备文件的主设备号 */

loff_t globalVar_llseek( struct file*, loff_t, int );
static ssize_t  globalVar_read( struct file*, char* __user, size_t, loff_t* );
static ssize_t  globalVar_write( struct file*, const char* __user, size_t, loff_t* );
static long     globalVar_ioctl( struct file*, unsigned int, unsigned long );
static int      globalVar_open( struct inode*, struct file* );
static int      globalVar_release( struct inode*, struct file* );

struct file_operations globalVar_fops = {
    owner:             THIS_MODULE,
    llseek:            globalVar_llseek,
    read:              globalVar_read,
    write:             globalVar_write,
    unlocked_ioctl:    globalVar_ioctl,
    open:              globalVar_open,
    release:           globalVar_release,
};

/* 用户空间的lseek()方法最终会调用本函数
 * 由于本示例在内核中使用2个全局变量分别作为当前读偏移量和当前写偏移量，据我分析
 * 不适合使用lseek()分别设置读/写偏移量，因此globalVar_llseek()函数仅作为示意，
 * 在其中没有具体设置操作。而将设置读写偏移量的操作放在globalVar_ioctl()函数中 */
loff_t globalVar_llseek( struct file* filp, loff_t offset, int whence ){
    printk( "call globalVar_llseek() func:\n" );
    printk( "offset is: %lld, whence is: %d\n", offset, whence );
    printk( "file.f_pos is: %lld\n", filp->f_pos );
    return 0;
}

/* 用户空间的ioctl()方法最终会调用本函数 
 * 若传入的cmd实参为0，表示将本字符设备文件的当前读偏移量赋值为arg实参
 * 若传入的cmd实参为1，表示将本字符设备文件的当前写偏移量赋值为arg实参 */
static long globalVar_ioctl( struct file* filp, unsigned int cmd, unsigned long arg ){
    printk( "======= globalVar_ioctl() start =======\n" );
    switch( cmd ){
    case 0:  /* 设置全局的读偏移量指针offset_read */
        printk( "set offset_read to %lu\n", arg );
        offset_read = arg;
        break;
    case 1:  /* 设置全局的写偏移量指针offset_write */
        printk( "set offset_write to %lu\n", arg );
        offset_write = arg;
        break;
    default:
        printk( "in globalVar_ioctl: bad cmd:%u\n", cmd );
    }
    printk( "======= globalVar_ioctl() end =======\n" );
    return 0;
}

/* 在双击打开设备文件或open()系统调用中，会调用本函数 */
static int globalVar_open( struct inode* inodp, struct file* filp ){

    printk( "======= call globalVar_open(), get this module =======\n" );
    if( !try_module_get(THIS_MODULE) ){
        printk( "Could not reserve module\n" );
        return -1;
    }

    offset_read  = 0;
    offset_write = 0;

    return 0;
}

/* 在关闭设备文件或close()系统调用中，会调用本函数 */
static int globalVar_release( struct inode* inodp, struct file* filp ){
    printk( "======= call globalVar_release(), put this module =======\n" );
    module_put( THIS_MODULE );

    return 0;
}

/*
 * 在使用编辑器编辑设备文件，或read()系统调用中，会调用本函数
 * 若本函数返回值为0，则表示已无数据可读，应用程序（比如cat命令或sublime文本编辑器等）不会再次调用本函数
 * 若本函数返回值非0，则表示可能还有数据待读，应用程序可能会再次调用本函数 */
static ssize_t globalVar_read( struct file* filp, char* __user buf, size_t len, loff_t* offset ){

	unsigned int slen;

    printk( "======= call globalVar_read func, len = %lu =======\n", len );
    printk( "offset arg   is: %lli\n", *offset );
    printk( "file->offset is: %lli\n",  filp->f_pos );
    printk( "private data is: %s\n", (char*)filp->private_data );
    printk( "offset_read  is: %u\n",    offset_read );

    slen = strlen( gBuf );  /* strlen()函数会返回缓冲区中字符串的长度，不包含该字符串末尾的null结束符 */
    slen++;                 /* 缓冲区中字符串长度需包含末尾的null结束符 */
    
    /* 若当前读写位置offset_read大于等于缓冲区字符串总长度，则表示已读到末尾 */
    if( offset_read >= slen )
    	return 0;  /* 当读函数返回0时,表示已无数据可读,应用程序(比如文本编辑器等)不会再次调用本函数,否则应用程序会再次调用本函数 */

    slen = slen - offset_read;
    
    printk( "slen is: %u\n", slen );
    
    copy_to_user( buf, gBuf, slen );
    offset_read += slen;    /* 在读偏移量上累加本次读取数据的长度 */

    printk( "offset_read changed to: %u\n", offset_read );

    return offset_read;  /* 
            * 读函数会返回所读出字符串的长度。经实践验证（在使用cat命令或sublime编辑器查看字符设备文件时），
            * 若返回值非0,则表示可能还有后续数据待读,应用程序会再次调用本函数;
            * 若返回值为0,则表示已无数据可读,应用程序不会再次调用本函数 */
}

/*
 * 在使用编辑器编辑设备文件，或write()系统调用中，会调用本函数
 * 若返回值不等于传入的len实参，则表示还有数据待写入，应用程序（比如echo命令或sublime
 * 编辑器等）会再次调用本函数；若返回值等于传入的len实参，则表示数据写入完毕，
 * 应用程序不会再次调用本函数。 */
static ssize_t globalVar_write( struct file* filp, const char* __user buf, size_t len, loff_t* offset ){

	unsigned int slen;

	printk( "======= call globalVar_write func, len = %lu =======\n", len );
    printk( "offset arg   is: %lli\n", *offset );
    printk( "file->offset is: %lli\n",  filp->f_pos );
    printk( "private data is: %s\n", (char*)filp->private_data );
    printk( "offset_write is: %u\n",    offset_write );

    /* 在开始写内存缓冲区之前,先清空该缓冲区 */
	if( offset_write == 0 )
		memset( gBuf, 0, sizeof(gBuf) );

    /* 该缓冲区最大长度为100 */
    slen = (len>100) ? 100 : len;
    printk( "slen is: %u\n", slen );

    /* 若当前的写位置加上要写入数据长度大于缓冲区最大长度,则从缓冲区首位置开始写入 */
    if( (offset_write + slen) > 99 )
    	offset_write = 0;
    
    copy_from_user( (gBuf+offset_write), buf, slen );
    offset_write += slen;       /* 在写偏移量上累加本次写入数据的长度 */

    printk( "offset_write changed to: %u\n", offset_write );

    return slen;  /* 
        * 写函数会返回所写入数据的长度。经实践验证（在使用echo命令或sublime编辑器写字符设备文件时），
        * 若返回值不等于传入的len实参，则说明还有数据待写入，应用程序会再次调用本函数；
        * 若返回值等于传入的len实参，则表示全部数据已写入完毕，应用程序不会再次调用本函数 */
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
    printk( "======= globalVar_init() start =======\n" );
    
    memset( gBuf, 0, sizeof(gBuf) );
    strcpy( gBuf, "hello faye\n" );
    
    cdevMajor = register_chrdev( 0, "globalVar", &globalVar_fops );
    if( cdevMajor < 0 ){
        printk( "register chrdev failure! ret = %+d\n", cdevMajor );
        return cdevMajor;
    }
    printk( "register chrdev success! cdevMajor is:%+d\n", cdevMajor );

    globalMem_bus_init( 0 );

    globalMem_device_init( cdevMajor );
    printk( "register globalMem_device success!\n" );
    
    globalMem_driver_init();
    printk( "register globalMem_driver success!\n" );
    
    printk( "======= globalVar_init() end =======\n" );
    return 0;
}
module_init( globalVar_init );

static void __exit globalVar_exit( void ){
    
    printk( "======= globalVar_exit() start =======\n" );
    
    globalMem_driver_exit();
    printk( "globalMem_driver exit!\n" );
    
    globalMem_device_exit( cdevMajor );
    printk( "globalMem_device exit!\n" );
    
    globalMem_bus_exit();
    printk( "globalMem_bus exit!\n" );
    
    unregister_chrdev( cdevMajor, "globalVar" );
    printk( "globalVar exit!\n" );
    
    printk( "======= globalVar_exit() end =======\n" );
}
module_exit( globalVar_exit );

