#include  <linux/kernel.h>
#include  <linux/module.h>
#include  <linux/init.h>
#include  <linux/string.h>
#include  <linux/device.h>

MODULE_AUTHOR( "faye" );
MODULE_LICENSE( "GPL" );  /*
    * 注意,本行不可省略,否则即使能成功编译,但在加载本模块时,
    * 会提示"Unknown symbol in module",并会在dmesg命令中给出所缺少的符号 */

//struct class * globalMem_class;

#if 1
/*
 * 由于内核中的bus_add_groups()函数未通过EXPORT_SYMBOL_GPL()导出（可参考本文中的
 *“attribute_group对象相关操作方法：”部分），因此在本模块文件中由用户自行定义
 * bus_add_groups()函数，而在bus_add_groups()函数实现中会使用subsys_private
 * 结构体定义，因此在此处给出subsys_private结构体定义。否则在编译时会报错：
 * error: dereferencing pointer to incomplete type ‘struct subsys_private’
 *      return sysfs_create_groups( &bus->p->subsys.kobj, groups );
 * subsys_private结构体定义在drivers/base/base.h文件中，但在ubuntu-16.04中未发现该
 * 头文件，因此也就无法通过include该头文件的方法来给出subsys_private结构体定义。
 * 是否还有更好的解决方法，尚待分析？？？？ */
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

/*
 * struct bus_type->match()函数指针会指向本函数。
 * 当向本bus下添加设备或驱动时会调用本函数进行匹配，若本函数返回值非0，则表示设备与驱动
 * 匹配成功，此时会调用bus_type->probe()方法；若本函数返回值为0，则表示匹配失败。  */
static int globalMem_bus_match( struct device *dev, struct device_driver *drv ){
    printk( "=== globalMem_bus_match() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "device_driver is: %s\n", drv->name );
    printk( "=== globalMem_bus_match() end ===\n" );
    return !strncmp( dev_name(dev), drv->name, strlen(drv->name) );
}

/* struct bus_type->uevent()函数指针会指向本函数，当在本总线下添加设备device对象时，会调用本函数 */
int globalMem_bus_uevent( struct device *dev, struct kobj_uevent_env *env ){
    printk( "=== globalMem_bus_uevent() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_uevent() end ===\n" );
    return 0;
}

/*
 * struct bus_type->probe()函数指针会指向本函数。若所加载的设备与驱动匹配成功
 * （bus_type->match()返回值非0），则会调用bus_type->probe()函数  */
static int globalMem_bus_probe( struct device *dev ){
    printk( "=== globalMem_bus_probe() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_probe() end ===\n" );
    return 0;
}

/*
 * struct bus_type->remove()函数指针会指向本函数。
 * 在移除本总线下已匹配的设备模块或驱动模块时，才会调用本函数
 * 在移除本总线下未匹配的设备模块或驱动模块时，不会调用本函数。 */
static int globalMem_bus_remove( struct device *dev ){
    printk( "=== globalMem_bus_remove() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_remove() end ===\n" );
    return 0;
}

static void globalMem_bus_shutdown( struct device *dev ){
    printk( "=== globalMem_bus_shutdown() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_bus_shutdown() end ===\n" );
}

/* 定义一个总线属性对象 */
static ssize_t globalMem_bus_author_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_bus_author );  /*
                       * snprintf()函数最多写入size-1个字符,并自动在字符串末尾加上'\0'结束符
                       * snprintf()函数会返回格式化后字符串的总长度，而不是实际写入缓冲区的长度，不包含‘\0’结束符
                       * 在输出字符设备文件缓冲区时,额外输出1个换行符,这样在命令行中输出该缓冲区内容(比如cat命令)时,可读性较好 */
}

static ssize_t globalMem_bus_author_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_author, PAGE_SIZE, "%s", buf );  /*
                       * 在保存字符设备文件缓冲区时,按原样格式保存,不附加其他字符 */
}

static BUS_ATTR( globalMem_bus_author, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_author_show, globalMem_bus_author_store ); /*
                                    * BUS_ATTR()宏会生成名为bus_attr_globalMem_bus_author的bus_attribute类型对象 */

static ssize_t globalMem_bus_attr_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_bus_attr );
}

static ssize_t globalMem_bus_attr_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attr, PAGE_SIZE, "%s", buf );
}

static BUS_ATTR( globalMem_bus_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attr_show, globalMem_bus_attr_store );

/*
 * 由于内核中的bus_add_groups()函数未通过EXPORT_SYMBOL_GPL()导出，
 * 因此在用户模块文件中自行定义该函数。 */
static int bus_add_groups( struct bus_type *bus, const struct attribute_group **groups ){
    return sysfs_create_groups( &bus->p->subsys.kobj, groups );
}

static ssize_t globalMem_bus_attrGroup_show( struct bus_type *bus, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_bus_attrGroup );
}

static ssize_t globalMem_bus_attrGroup_store( struct bus_type *bus, const char *buf, size_t count ){
    return snprintf( globalMem_bus_attrGroup, PAGE_SIZE, "%s", buf );
}

/* 定义一个名为bus_attr_globalMem_bus_attrGroup的bus_attribute对象 */
static BUS_ATTR( globalMem_bus_attrGroup, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_bus_attrGroup_show, globalMem_bus_attrGroup_store );

/* 定义一个attribute_group对象，并将其.attrs字段赋值为bus_attr_globalMem_bus_attrGroup.attr字段 */
struct attribute_group  globalMem_bus_attrGroup_set = {
    .name  = "globalMem_bus_attrGroup_name",
    .attrs = (struct attribute*[]){ &(bus_attr_globalMem_bus_attrGroup.attr), NULL },
};

struct bus_type globalMem_bus = {
    .name       = "globalMem_bus",
    .match      =  globalMem_bus_match,
    .uevent     =  globalMem_bus_uevent,
    .shutdown   =  globalMem_bus_shutdown,
    .probe      =  globalMem_bus_probe,
    .remove     =  globalMem_bus_remove,
    //.bus_groups = ( const struct attribute_group*[] ){ &globalMem_bus_attrGroup_set, NULL },
                                  /* 可直接对本字段进行赋值，或通过bus_add_groups()函数对本文件进行赋值 */
};
EXPORT_SYMBOL( globalMem_bus );

/* 总线device对象相关 */

/*
 * struct globalMem__busDevice->release()函数指针会指向本函数
 * 当device对象的引用计数减至0时，会在device_release()函数中调用dev->release()函数  */
void globalMem_busDevice_release( struct device *dev ){
    printk( "=== globalMem_busDevice_release() start ===\n" );
    printk( "device is: %s\n", dev_name(dev) );
    printk( "=== globalMem_busDevice_release() end ===\n" );
}

//struct device * globalMem_busDevice;
#if 1
/* 用户自定义总线所对应的device对象，作为本总线下所有设备的父设备 */
struct device globalMem_busDevice = {
    .init_name  = "globalMem_busDevice",
    .bus        = &globalMem_bus,
    .release    =  globalMem_busDevice_release,
};
EXPORT_SYMBOL( globalMem_busDevice );
#endif

static ssize_t globalMem_busDevice_attr_show( struct device *dev, struct device_attribute *attr, char *buf ){
    return snprintf( buf, PAGE_SIZE, "%s\n", globalMem_busDev_attr );
}

static ssize_t globalMem_busDevice_attr_store( struct device *dev, struct device_attribute *attr, const char *buf, size_t count ){
    return snprintf( globalMem_busDev_attr, PAGE_SIZE, "%s", buf );
}

static DEVICE_ATTR( globalMem_busDevice_attr, (S_IRUGO|S_IWUSR|S_IWGRP), globalMem_busDevice_attr_show, globalMem_busDevice_attr_store );

int __init globalMem_bus_init( unsigned int cdevMajor ){
    
    //dev_t devNo;
    int ret;
    
    printk( "******* globalMem_bus_init() start *******\n" );

    ret = bus_register( &globalMem_bus );
    if( ret ){
        printk( "bus_register() failed: %d\n", ret );
        return ret;
    }
    
    if( bus_create_file( &globalMem_bus, &bus_attr_globalMem_bus_author ) )
        printk( "Unable to create globalMem_bus author attribute file\n" );
    if( bus_create_file( &globalMem_bus, &bus_attr_globalMem_bus_attr ) )
        printk( "Unable to create globalMem_bus attribute file\n " );

    bus_add_groups( &globalMem_bus, ( const struct attribute_group*[] ){ &globalMem_bus_attrGroup_set, NULL } );

    printk( "globamMem_bus register success!\n" );

#if 0
    globalMem_class = class_create( THIS_MODULE, "globalMem_class" );
    
    devNo = MKDEV( cdevMajor, 0 );
    globalMem_busDevice = device_create( globalMem_class, NULL, devNo, NULL, "globalMem_busDevice" );
    if( IS_ERR(globalMem_busDevice) ){
        printk( "device_create() failed: %ld\n", PTR_ERR(globalMem_busDevice) );
        printk( "devNo is: %u\n", devNo );
        printk( "Major is: %u\n", MAJOR( devNo ) );
        return -1;
    }
#endif

#if 1
    //globalMem_busDevice.devt = MKDEV( cdevMajor, 0 );

    if( device_register( &globalMem_busDevice ) )
        printk( "register globalMem_busDevice fail!\n" );

    if( device_create_file( &globalMem_busDevice, &dev_attr_globalMem_busDevice_attr ) )
        printk( "Unable to create globalMem_busDevice attribute file\n" );

    printk( "Major is: %u\n", MAJOR(globalMem_busDevice.devt) );
    printk( "Minor is: %u\n", MINOR(globalMem_busDevice.devt) );

    globalMem_bus.dev_root = &globalMem_busDevice; /*
                * 在注册完总线device对象后，将其所属总线的bus_type->dev_root字段指向该device对象。
                * 注意，若设置了总线bus对象的dev_root字段，则在调用bus_unregister()注销该总线过程中，
                * 会自动调用device_unregister()函数注销dev_root字段所指向的device对象。
                * 也就是说，若dev_root字段被设置，则在调用bus_unregister()函数之前，
                * 无需调用device_unregister()函数注销dev_root字段所指向的device对象。 */

    printk( "globalMem_busDevice register success!\n" );
#endif

    printk( "******* globalMem_bus_init() end *******\n" );
    
    return ret;
}
EXPORT_SYMBOL( globalMem_bus_init );

void __exit globalMem_bus_exit( void ){
    printk( "******* globalMem_bus_exit() start *******\n" );
    //device_unregister( &globalMem_busDevice );
    bus_unregister( &globalMem_bus );
    //class_destroy( globalMem_class );
    printk( "******* globalMem_bus_exit() end *******\n" );
}
EXPORT_SYMBOL( globalMem_bus_exit );

