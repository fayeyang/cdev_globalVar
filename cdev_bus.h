#ifndef cdev_bus_h
#define cdev_bus_h

extern struct bus_type globalMem_bus;
extern struct device globalMem_busDevice;

extern int __init globalMem_bus_init( unsigned int cdevMajor );
extern void __exit globalMem_bus_exit( void );

#endif
