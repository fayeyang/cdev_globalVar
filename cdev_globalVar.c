#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );

extern int __init globalMem_bus_init( unsigned int cdevMajor );
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

	unsigned int slen;

    printk( "======= call globalVar_read func, len = %lu =======\n", len );
    printk( "offset arg   is: %lli\n", *offset );
    printk( "file->offset is: %lli\n",  filp->f_pos );
    printk( "private data is: %s\n", (char*)filp->private_data );
    printk( "offset_read  is: %u\n",    offset_read );

    slen = strlen( gBuf );  /* strlen()函数会返回缓冲区中字符串的长度，不包含该字符串末尾的null结束符 */
    slen++;                 /* 缓冲区中字符串长度需包含末尾的null结束符 */
    if( offset_read >= slen )
    	return 0;  /* 当读函数返回0时,表示已无数据可读,应用程序不会再次调用本函数,否则应用程序会再次调用本函数 */

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

static ssize_t globalVar_write( struct file* filp, const char* __user buf, size_t len, loff_t* offset ){

	unsigned int slen;

	printk( "+++++++ call globalVar_write func, len = %lu +++++++\n", len );
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
    memset( gBuf, 0, sizeof(gBuf) );
    strcpy( gBuf, "hello faye\n" );
    
    cdevMajor = register_chrdev( 0, "globalVar", &globalVar_fops );
    if( cdevMajor < 0 ){
        printk( "register chrdev failure! ret = %+d\n", cdevMajor );
        return cdevMajor;
    }
    
    globalMem_bus_init( cdevMajor );
    
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

