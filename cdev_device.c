#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );

extern struct bus_type globalMem_bus;
extern struct device globalMem_busDevice;

char globalMem_device_attr[ PAGE_SIZE ] = "globalMem_device_attr";
char globalMem_device_attrGroup[ PAGE_SIZE ] = "globalMem_device_attrGroup";

void globalMem_device_release( struct device *dev ){
    printk( "####### globalMem_device_release() start #######\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "####### globalMem_device_release() end #######\n" );
}

static ssize_t globalMem_device_attr_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, PAGE_SIZE-2, "%s\n", globalMem_device_attr );
}

static ssize_t globalMem_device_attr_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_device_attr, PAGE_SIZE-2, "%s\n", buf );    
}

static DEVICE_ATTR( globalMem_device_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_device_attr_show, globalMem_device_attr_store );



