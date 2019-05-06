obj-m += rd_module.o

build_module: rd_module.c
	make -C /usr/src/linux-`uname -r` SUBDIRS=$(PWD) modules

load: build_module
	insmod rd_module.ko

unload:
	rmmod rd_module.ko
