#ifndef cdev_device_h
#define cdev_device_h

extern int __init globalMem_device_init( unsigned int cdevMajor );
extern void globalMem_device_exit( unsigned int cdevMajor );

#endif