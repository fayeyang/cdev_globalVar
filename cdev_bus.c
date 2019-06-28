#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );

#if 1
struct subsys_private {
    struct kset subsys;
    struct kset *devices_kset;
    struct list_head interfaces;
    struct mutex mutex;
    struct kset *drivers_kset;
    struct klist klist_devices;
    struct klist klist_drivers;
    struct blocking_notifier_head bus_notifier;
    unsigned int drivers_autoprobe:1;
    struct bus_type *bus;
    struct kset glue_dirs;
    struct class *class;
};
#endif

char globalMem_bus_author[ PAGE_SIZE ]    = "FayeYang";
char globalMem_bus_attr[ PAGE_SIZE ]      = "globalMem bus attr";
char globalMem_bus_attrGroup[ PAGE_SIZE ] = "globalMem bus attrGroup";
char globalMem_busDev_attr[ PAGE_SIZE ]   = "globalMem busDev attr";

static int globalMem_bus_match( struct device *dev, struct device_driver *drv ){
    printk( "=== globalMem_bus_match() start === " );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "device_driver is: %s\n", drv->name );
    printk( "=== globalMem_bus_match() end ===" );
    return !strncmp( dev_name(dev), drv->name, strlen(drv->name) );
}

static int globalMem_bus_uevent( struct device *dev, struct kobj_uevent_env *env ){
    printk( "=== globalMem_bus_uevent() start ===" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_uevent() end ===" );
    return 0;
}

static int globalMem_bus_probe( struct device *dev ){
    printk( "=== globalMem_bus_probe() start ===" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_probe() end ===" );
    return 0;
}

static int globalMem_bus_remove( struct device *dev ){
    printk( "=== globalMem_bus_remove() start ===" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_remove() end ===" );
    return 0;
}

static void globalMem_bus_shutdown( struct device *dev ){
    printk( "=== globalMem_bus_shutdown() start ===" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_shutdown() end ===" );
}

/* 定义一个总线属性对象 */
static ssize_t globalMem_bus_author_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_author );  /*
                                * snprintf()函数最多写入size-1个字符,并自动在字符串末尾加上结束符
                                * 最极端的情况,字符串加1个换行符加1个自动添加的结束符,因此为size-2 */
}

static ssize_t globalMem_bus_author_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_author, (PAGE_SIZE-2), "%s\n", buf );
}

static BUS_ATTR( globalMem_bus_author, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_author_show, globalMem_bus_author_store ); /*
                                    * BUS_ATTR()宏会在本处会生成名为bus_attr_faye_bus_author的bus_attribute类型对象 */

static ssize_t globalMem_bus_attr_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_attr );
}

static ssize_t globalMem_bus_attr_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attr, (PAGE_SIZE-2), "%s\n", buf );
}

static BUS_ATTR( globalMem_bus_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attr_show, globalMem_bus_attr_store );

static int bus_add_groups( struct bus_type *bus, const struct attribute_group **groups ){
    return sysfs_create_groups( &bus->p->subsys.kobj, groups );
}

static ssize_t globalMem_bus_attrGroup_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_attrGroup );
}

static ssize_t globalMem_bus_attrGroup_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attrGroup, (PAGE_SIZE-2), "%s\n", buf );
}

/* 定义一个bus_attribute对象,并将其封装到attribute_group对象中 */
static BUS_ATTR( globalMem_bus_attrGroup, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attrGroup_show, globalMem_bus_attrGroup_store );

const struct attribute_group  globalMem_bus_attrGroup_set = {
    .name  = "globalMem_bus_attrGroup_name",
    .attrs = (struct attribute*[]){ &(bus_attr_globalMem_bus_attrGroup.attr), NULL },
};

struct bus_type globalMem_bus = {
    .name       = "globalMem",
    .match      =  globalMem_bus_match,
    .uevent     =  globalMem_bus_uevent,
    .shutdown   =  globalMem_bus_shutdown,
    .probe      =  globalMem_bus_probe,
    .remove     =  globalMem_bus_remove,
    //.bus_groups = ( const struct attribute_group*[] ){ &globalMem_bus_attrGroup_set, NULL },
};
EXPORT_SYMBOL( globalMem_bus );

/* 总线device对象相关 */

void globalMem_busDevice_release( struct device *dev ){
    printk( "=== globalMem_busDevice_release() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_busDevice_release() end ===" );
}

struct device globalMem_busDevice = {
    .init_name  = "globalMem_busDevice",
    .bus        = &globalMem_bus,
    .release    =  globalMem_busDevice_release,
};
EXPORT_SYMBOL( globalMem_busDevice );

static ssize_t globalMem_busDevice_attr_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_busDev_attr );
}

static ssize_t globalMem_busDevice_attr_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_busDev_attr, (PAGE_SIZE-2), "%s\n", buf );
}

static DEVICE_ATTR( globalMem_busDevice_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_busDevice_attr_show, globalMem_busDevice_attr_store );

int globalMem_bus_init( void ){
    int ret;
    
    printk( "******* globalMem_bus_init() start *******\n" );

    ret = bus_register( &globalMem_bus );
    if( ret )
        return ret;
    
    bus_add_groups( &globalMem_bus, ( const struct attribute_group*[] ){ &globalMem_bus_attrGroup_set, NULL } );

    if( bus_create_file( &globalMem_bus, &bus_attr_globalMem_bus_author ) )
        printk( "Unable to create globalMem_bus author attribute file\n" );
    if( bus_create_file( &globalMem_bus, &bus_attr_globalMem_bus_attr ) )
        printk( "Unable to create globalMem_bus attribute file\n " );

    printk( "globamMem_bus register success!\n" );
    
    if( device_register( &globalMem_busDevice ) )
        printk( "register globalMem_busDevice fail!\n" );

    if( device_create_file( &globalMem_busDevice, &dev_attr_globalMem_busDevice_attr ) )
        printk( "Unable to create globalMem_busDevice attribute file\n" );

    globalMem_bus.dev_root = &globalMem_busDevice;
    printk( "globalMem_busDevice register success!\n" );
    printk( "******* globalMem_bus_init() end *******\n" );
    
    return ret;
}
EXPORT_SYMBOL( globalMem_bus_init );

void globalMem_bus_exit( void ){
    printk( "******* globalMem_bus_exit() start *******\n" );
    //device_unregister( &globalMem_busDevice );
    printk( "globalMem_busDevice is: %lu\n", (unsigned long)&globalMem_bus );
    bus_unregister( &globalMem_bus );
    printk( "******* globalMem_bus_exit() end *******\n" );
}
EXPORT_SYMBOL( globalMem_bus_exit );

//module_init( globalMem_bus_init );
//module_exit( globalMem_bus_exit );

