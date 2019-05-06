obj-m += rd_module.o

build_module: rd_module.c
	make -C /usr/src/linux-`uname -r` SUBDIRS=$(PWD) modules

load: build_module
	insmod rd_module.ko

unload:
	rmmod rd_module.ko

test:
	gcc -o rd_interface.o -c rd_interface.c; gcc test_routine.c rd_interface.o -o test_routine; ./test_routine

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean; rm test_routine
