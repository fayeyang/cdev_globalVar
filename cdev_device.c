#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>
#include  <linux/sysfs.h>

#include  "cdev_bus.h"

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );  /*
    * 注意,本行不可省略,否则即使能成功编译,但在加载本模块时,
    * 会提示"Unknown symbol in module",并会在dmesg命令中给出所缺少的符号 */

struct class  * globalMem_class;
struct device * globalMem_device_0;
struct device * globalMem_device_1;

char globalMem_device_attr[ PAGE_SIZE ]      = "globalMem_device_attr";
char globalMem_device_attrGroup[ PAGE_SIZE ] = "globalMem_device_attrGroup";

/*
 * globalMem_device.release函数指针会指向本函数.
 * 当device对象的引用计数减至0时，会在device_release()函数中调用dev->release()函数
 */
void globalMem_device_release( struct device *dev ){
    printk( "####### globalMem_device_release() start #######\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "####### globalMem_device_release() end #######\n" );
}

/* struct class.dev_uevent函数指针会指向本函数，当在本class下添加设备device对象时，会调用本函数 */
int globalMem_class_uevent( struct device *dev, struct kobj_uevent_env *env ){
    printk( "=== globalMem_class_uevent() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_class_uevent() end ===\n" );
    return 0;
}

int device_add_groups( struct device *dev, const struct attribute_group **groups ){
    return sysfs_create_groups( &dev->kobj, groups );
}

static ssize_t globalMem_device_attrGroup_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_device_attrGroup );
}

static ssize_t globalMem_device_attrGroup_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_device_attrGroup, PAGE_SIZE, "%s", buf );    
}

static DEVICE_ATTR( globalMem_device_attrGroup, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_device_attrGroup_show, globalMem_device_attrGroup_store );  /*
            * DEVICE_ATTR()宏会定义一个名为dev_attr_globalMem_device_attrGroup的device_attribute对象 */

/* 定义1个attribute_group对象,将其attr字段赋值为dev_attr_globalMem_device_attrGroup.attr字段 */
const struct attribute_group globalMem_device_attrGroup_set = {
    .name = "globalMem_device_attrGroup_set",
    .attrs = (struct attribute*[]){ &(dev_attr_globalMem_device_attrGroup.attr), NULL },
};

static ssize_t globalMem_device_attr_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_device_attr );  /*
                       * snprintf()函数最多写入size-1个字符,并自动在字符串末尾加上'\0'结束符
                       * snprintf()函数会返回格式化后字符串的总长度，而不是实际写入缓冲区的长度，不包含‘\0’结束符
                       * 在输出字符设备文件缓冲区时,额外输出1个换行符,这样在命令行中输出该缓冲区内容(比如cat命令)时,可读性较好 */
}

static ssize_t globalMem_device_attr_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_device_attr, PAGE_SIZE, "%s", buf );  /*
                       * 在保存字符设备文件缓冲区时,按原样格式保存,不附加其他字符 */
}

static DEVICE_ATTR( globalMem_device_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_device_attr_show, globalMem_device_attr_store );


#if 0
int __init globalMem_device_init( unsigned int cdevMajor ){
    
    int ret;

    printk( "======= globalMem_device_init() start =======\n" );
    
    globalMem_class = class_create( THIS_MODULE, "globalMem_class" );
    if( IS_ERR( globalMem_class ) ){
        printk( "class_create() failed: %ld\n", PTR_ERR(globalMem_class) );
        return -1;
    }
    globalMem_class->dev_uevent  = globalMem_class_uevent;
    globalMem_class->dev_release = globalMem_device_release;
    printk( "create globalMem_class success!\n" );
    
    globalMem_device_0 = device_create( globalMem_class, &globalMem_busDevice, MKDEV(cdevMajor,0), NULL, "globalMem_device_0" );
    if( IS_ERR( globalMem_device_0 ) ){
        printk( "device_create() failed: %ld\n", PTR_ERR(globalMem_device_0) );
        return -1;
    }
    
    ret = device_create_file( globalMem_device_0, &dev_attr_globalMem_device_attr );
    if( ret ){
        printk( "create globalMem_device_0 attribute file failed!\n" );
        return ret;
    }
    
    ret = device_add_groups( globalMem_device_0, ( const struct attribute_group*[] ){ &globalMem_device_attrGroup_set, NULL } );
    if( ret ){
        printk( "create globalMem_device_0 attribute group file failed!\n" );
        return ret;
    }
    
    printk( "create globalMem_device_0 success!\n" );
    
    globalMem_device_1 = device_create( globalMem_class, &globalMem_busDevice, MKDEV(cdevMajor,1), NULL, "globalMem_device_1" );
    if( IS_ERR( globalMem_device_1 ) ){
        printk( "device_create() failed: %ld\n", PTR_ERR(globalMem_device_1) );
        return -1;
    }
    
    ret = device_create_file( globalMem_device_1, &dev_attr_globalMem_device_attr );
    if( ret ){
        printk( "create globalMem_device_1 attribute file failed!\n" );
        return ret;
    }
    
    if( device_add_groups( globalMem_device_1, ( const struct attribute_group*[] ){ &globalMem_device_attrGroup_set, NULL } ) ){
        printk( "create globalMem_device_1 attribute group file failed!\n" );
        return -1;
    }
    
    printk( "create globalMem_device_1 success!\n" );
    
    printk( "======= globalMem_device_init() end =======\n" );
    return 0;
}
EXPORT_SYMBOL( globalMem_device_init );
#endif

#if 1
struct device globalMem_device = {
    .init_name  = "globalMem_device",
    .bus        = &globalMem_bus,
    .parent     = &globalMem_busDevice,
    .release    =  globalMem_device_release,
    .groups     = (const struct attribute_group*[] ){ &globalMem_device_attrGroup_set, NULL },
};

int __init globalMem_device_init( unsigned int cdevMajor ){
    
    int ret;
    printk( "======= globalMem_device_init() start =======\n" );

    globalMem_device.devt = MKDEV( cdevMajor, 0 );

    ret = device_register( &globalMem_device );
    if( ret ){
        printk( KERN_DEBUG "Unable to register device\n" );
        return ret;
    }

    ret = device_create_file( &globalMem_device, &dev_attr_globalMem_device_attr );
    if( ret ){
        printk( KERN_DEBUG "Unable to create device attribute file\n" );
        return ret;
    }

    printk( KERN_DEBUG "globalMem_devie register success\n" );
    printk( "======= globalMem_device_init() end =======\n" );
    return 0;
}
#endif

void __exit globalMem_device_exit( unsigned int cdevMajor ){
    
    printk( "======= globalMem_device_exit() start =======\n" );
    
    device_unregister( &globalMem_device );
    
    //device_destroy( globalMem_class, MKDEV(cdevMajor, 0) );
    //device_destroy( globalMem_class, MKDEV(cdevMajor, 1) );
    
    //class_destroy( globalMem_class );
    
    printk( "======= globalMem_device_exit() end =======\n" );
}
EXPORT_SYMBOL( globalMem_device_exit );

