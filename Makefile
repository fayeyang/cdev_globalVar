ifneq ($(KERNELRELEASE),)
#aaobjs:=$(subst $(usrPwd),,$(aaobjs))
#aaobjs:=$(notdir $(aaobjs))
obj-m:=cdev_globalVar_Module.o
cdev_globalVar_Module-objs:=$(usrObjs)
else
#usrPwd:=/home/faye/work/work_normal/gcc_test/device_driver_test
usrSrc:=$(wildcard *.c)
usrSrc:=$(patsubst %.mod.c,,$(usrSrc))
usrObjs:=$(usrSrc:.c=.o)
usrSrc:=$(wildcard *.cpp)
usrSrc:=$(patsubst %.mod.cpp,,$(usrSrc))
usrObjs+=$(usrSrc:.cpp=.o)
export usrObjs
KERNELDIR?=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all:clean
	@echo objs=$(usrObjs)
	@echo MAKE = $(MAKE)
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(MAKE) -C test/

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

.PHONY:clean
clean:
	$(MAKE) -C test/ clean
	rm -rf *.o *.ko *.mod.c *.mod.o .tmp_versions/ .*.cmd *.order .*.mk
endif




