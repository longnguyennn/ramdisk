obj-m += rd_module.o

all: build remove install run

build:
	make -C /usr/src/linux-`uname -r` SUBDIRS=$(PWD) modules

remove:
	-rmmod rd_module

install:
	insmod rd_module.ko

run:
	gcc test_my_getchar.c -o test_my_getchar
	./test_my_getchar

clean:
	rm my_getchar.mod.* my_getchar.ko my_getchar.o Module.* modules.*
	rm test_my_getchar