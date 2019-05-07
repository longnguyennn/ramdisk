#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include "rd_interface.h"
#include "rd.h"

int rd_creat(char * pathname, mode_t mode) {

	int fd = open ( "/proc/ramdisk", O_RDONLY );

	creat_arg_t arg;
	strcpy(arg.path_name, pathname);
	arg.mode = mode;

	ioctl (fd, RD_CREAT, &arg);

	close (fd);

	return arg.retval;
}

int rd_mkdir(char *pathname) {
	int fd = open("/proc/ramdisk", O_RDONLY );
	mkdir_arg_t arg;
	strcpy(arg.path_name, pathname);

	ioctl(fd, RD_MKDIR, &arg);

	close(fd);

	return arg.retval;
}

int rd_open(char *pathname, int flags) {
	int fd = open("/proc/ramdisk", O_RDONLY );
	open_arg_t arg;
	strcpy(arg.path_name, pathname);
	arg.flags = flags;
	arg.pid = getpid();
	ioctl(fd, RD_OPEN, &arg);

	close(fd);

	return arg.retval;
}
