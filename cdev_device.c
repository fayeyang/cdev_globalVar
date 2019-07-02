#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );  /*
    * 注意,本行不可省略,否则即使能成功编译,但在加载本模块时,
    * 会提示"Unknown symbol in module",并会在dmesg命令中给出所缺少的符号 */

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

static ssize_t globalMem_device_attrGroup_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_device_attrGroup );
}

static ssize_t globalMem_device_attrGroup_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_device_attrGroup, PAGE_SIZE, "%s", buf );    
}

static DEVICE_ATTR( globalMem_device_attrGroup, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_device_attrGroup_show, globalMem_device_attrGroup_store );  /*
            * DEVICE_ATTR()宏会定义一个名为dev_attr_globalMem_device_attrGroup的device_attribute对象 */

/* 定义1个attribute_group对象,将其attr字段赋值为dev_attr_globalMem_device_attrGroup.attr字段 */
struct attribute_group globalMem_device_attrGroup_set = {
    .name = "globalMem_device_attrGroup_set",
    .attrs = (struct attribute*[]){ &(dev_attr_globalMem_device_attrGroup.attr), NULL },
};

#if 0
struct device globalMem_device = {
    .init_name = "globalMem_device",
    .bus       = &globalMem_bus,
    .release   =  globalMem_device_release,
    //.groups    = (const struct attribute_group*[]){ &globalMem_device_attrGroup_set, NULL }
};
#endif

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

static int __init globalMem_device_init( unsigned int cdevMajor ){
    
    int ret;
    struct class  * globalMem_class;
    struct device * globalMem_device_0;
    struct device * globalMem_device_1;
    
    printk( "======= globalMem_device_init() start =======\n" );
    
    globalMem_class = class_create( THIS_MODULE, "globalMem_class" );
    if( IS_ERR( globalMem_class ) ){
        printk( "class_create() failed: %ld\n", PTR_ERR(globalMem_class) );
        return -1;
    }
    
    globalMem_device_0 = device_create( globalMem_class, NULL, MKDEV(cdevMajor,0), NULL, "globalMem_device_0" );
    if( IS_ERR( globalMem_device_0 ) ){
        printk( "device_create() failed: %ld\n", PTR_ERR(globalMem_device_0) );
        return -1;
    }
    
    if( device_create_file( globalMem_device_0, &dev_attr_globalMem_device_attr ) ){
        printk( "create globalMem_device_0 attribute file failed!\n" );
    }
    
    if( device_add_groups( globalMem_device_0, ( const struct attribute_group*[] ){ &globalMem_device_attrGroup_set, NULL } ) ){
        printk( "create globalMem_device_0 attribute group file failed!\n" );
    }
    
    globalMem_device_1 = device_create( globalMem_class, NULL, MKDEV(cdevMajor,1), NULL, "globalMem_device_1" );
    if( IS_ERR( globalMem_device_1 ) ){
        printk( "device_create() failed: %ld\n", PTR_ERR(globalMem_device_1) );
        return -1;
    }
    
    if( device_create_file( globalMem_device_1, &dev_attr_globalMem_device_attr ) ){
        printk( "create globalMem_device_1 attribute file failed!\n" );
    }
    
    if( device_add_groups( globalMem_device_1, ( const struct attribute_group*[] ){ &globalMem_device_attrGroup_set, NULL } ) ){
        printk( "create globalMem_device_1 attribute group file failed!\n" );
    }
    
    printk( "======= globalMem_device_init() end =======\n" );
    return 0;
}

