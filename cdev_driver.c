#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>
#include  <linux/sysfs.h>

#include "cdev_bus.h"

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );  /*
    * 注意,本行不可省略,否则即使能成功编译,但在加载本模块时,
    * 会提示"Unknown symbol in module",并会在dmesg命令中给出所缺少的符号 */

struct driver_private {
    struct kobject         kobj;
    struct klist           klist_devices;
    struct klist_node      knode_bus;
    struct module_kobject *mkobj;
    struct device_driver  *driver;
};

char globalMem_driver_author[ PAGE_SIZE ]     = "fayeYang";
char globalMem_driver_attr[ PAGE_SIZE ]       = "globalMem_driver_attr";
char globalMem_driver_attr_group[ PAGE_SIZE ] = "globalMem_driver_attr_group";

int globalMem_driver_remove( struct device *dev ){
    printk( "*** in globalMem_driver_remove() start ***\n" );
    printk(  "device is:%s\n", dev_name(dev) );
    printk( "*** in globalMem_driver_remove() end ***\n" );
    return 0;
}

int globalMem_driver_probe( struct device *dev ){
    printk( "*** in globalMem_driver_probe() start ***\n" );
    printk(  "device is:%s\n", dev_name(dev) );
    printk( "*** globalMem driver can handle the device! ***\n" );
    printk( "*** in globalMem_driver_probe() end ***\n" );
    return 0;
}

void globalMem_driver_shutdown( struct device *dev ){
    printk( "*** in globalMem_driver_shutdown() start ***\n" );
    printk(  "device is:%s\n", dev_name(dev) );
    printk( "*** in globalMem_driver_shutdown() end ***\n" );
}

static ssize_t globalMem_driver_author_show( struct device_driver *drv, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_driver_author );
}

static ssize_t globalMem_driver_author_store( struct device_driver *drv, const char *buf, size_t count ){
    return snprintf( globalMem_driver_author, PAGE_SIZE, "%s\n", buf );
}

static DRIVER_ATTR_RW( globalMem_driver_author );


static ssize_t globalMem_driver_attribute_show( struct device_driver *drv, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_driver_attr );
}

static ssize_t globalMem_driver_attribute_store( struct device_driver *drv, const char *buf, size_t count ){
    return snprintf( globalMem_driver_attr, PAGE_SIZE, "%s\n", buf );
}

static DRIVER_ATTR_RW( globalMem_driver_attribute );

int driver_add_groups( struct device_driver *drv, const struct attribute_group **groups ){
    return sysfs_create_groups( &drv->p->kobj, groups );
}

static ssize_t globalMem_driver_attrGroup_show( struct device_driver *drv, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_driver_attr_group );
}

static ssize_t globalMem_driver_attrGroup_store( struct device_driver *drv, const char *buf, size_t count ){
    return snprintf( globalMem_driver_attr_group, PAGE_SIZE, "%s\n", buf );
}

static DRIVER_ATTR_RW( globalMem_driver_attrGroup );

struct attribute_group globalMem_driver_attrGroup_set = {
    .name  = "globalMem_driver_attrGroup_set",
    .attrs = (struct attribute*[]){ &(driver_attr_globalMem_driver_attrGroup.attr), NULL },
};

struct device_driver globalMem_driver = {
    .name     = "globalMem_busDevice",
    .bus      = &globalMem_bus,
    .probe    =  globalMem_driver_probe,
    .remove   =  globalMem_driver_remove,
    .shutdown =  globalMem_driver_shutdown,
};

int __init globalMem_driver_init( void ){
    
    int ret;
    
    printk( "======= globalMem_driver_init start =======\n" );
    
    ret = driver_register( &globalMem_driver );
    if( ret ){
        printk( "driver_register() failed!\n" );
        return ret;
    }
    
    ret = driver_create_file(  &globalMem_driver, &driver_attr_globalMem_driver_author);
    if( ret ){
        printk( "driver create author file failed!\n" );
        return ret;
    }
    
    ret = driver_create_file( &globalMem_driver, &driver_attr_globalMem_driver_attribute );
    if( ret ){
        printk( "driver create attribute file failed!\n" );
        return ret;
    }
    
    driver_add_groups( &globalMem_driver, ( const struct attribute_group*[] ){ &(globalMem_driver_attrGroup_set), NULL } );
    
    printk( "globalMem_driver register success!\n" );
    printk( "======= globalMem_driver_init end =======\n" );
    
    return 0;
}

void __exit globalMem_driver_exit( void ){
    printk( "======= globalMem_driver_exit() start =======\n" );
    
    driver_unregister( &globalMem_driver );
    
    printk( "globalMem_driver unregistered!\n" );
    printk( "======= globalMem_driver_end() end =======\n" );
}
