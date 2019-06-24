#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>

MODULE_LICENSE( "GPL" );

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

static ssize_t globalMem_bus_author_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_author );  /*
                                * snprintf()函数最多写入size-1个字符,并自动在字符串末尾加上结束符
                                * 最极端的情况,字符串加1个换行符加1个自动添加的结束符,因此为size-2 */
}

static ssize_t globalMem_bus_author_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_author, (PAGE_SIZE-2), "%s\n", buf );
}

static BUS_ATTR( globalMem_bus_author, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_author_show, globalMem_bus_author_store );

static ssize_t globalMem_bus_attr_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_attr );
}

static ssize_t globalMem_bus_attr_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attr, (PAGE_SIZE-2), "%s\n", buf );
}

static BUS_ATTR( globalMem_bus_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attr_show, globalMem_bus_attr_store );

static ssize_t globalMem_bus_attrGroup_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, (PAGE_SIZE-2), "%s\n", globalMem_bus_attrGroup );
}

static ssize_t globalMem_bus_attrGroup_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attrGroup, (PAGE_SIZE-2), "%s\n", buf );
}

static BUS_ATTR( globalMem_bus_attrGroup, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attrGroup_show, globalMem_bus_attrGroup_store );

struct attribute_group globalMem_bus_attrGroup_set = {
    .name  = "globalMem_bus_attrGroup_name",
    .attrs = (struct attribute*[]){ &(bus_attr_globalMem_bus_attrGroup.attr), NULL },
};

struct bus_type globalMem_bus = {
    .name = "globalMem",
    .match = globalMem_bus_match,
    .uevent = globalMem_bus_uevent,
    .shutdown = globalMem_bus_shutdown,
    .probe = globalMem_bus_probe,
    .remove = globalMem_bus_remove,
    .bus_groups = ( const struct attribute_group*[] ){ &globalMem_bus_attrGroup_set, NULL },
};
EXPORT_SYMBOL( globalMem_bus );




