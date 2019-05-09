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

int rd_close(int input_fd) {
	int fd = open("/proc/ramdisk", O_RDONLY );
	close_arg_t arg;
	arg.fd = input_fd;
	arg.pid = getpid();

	ioctl(fd, RD_CLOSE, &arg);
	
	close(fd);

	return arg.retval;
}

int rd_read(int input_fd, char * address, int num_bytes) {
	int fd = open("/proc/ramdisk", O_RDONLY);
	read_arg_t arg;
	arg.fd = input_fd;
	arg.address = address;
	arg.num_bytes = num_bytes;
	arg.pid = getpid();

	ioctl(fd, RD_READ, &arg);
	close(fd);

	return arg.retval;
}

int rd_write(int input_fd, char * address, int num_bytes) {
	int fd = open("/proc/ramdisk", O_RDONLY);
	write_arg_t arg;
	arg.fd = input_fd;
	arg.address = address;
	arg.num_bytes = num_bytes;
	arg.pid = getpid();

	ioctl(fd, RD_WRITE, &arg);
	close(fd);

	return arg.retval;
}

int rd_lseek(int input_fd, int offset) {
	int fd = open("/proc/ramdisk", O_RDONLY );
	lseek_arg_t arg;
	arg.fd = input_fd;
	arg.pid = getpid();
	arg.offset = offset;

	ioctl(fd, RD_LSEEK, &arg);

	close(fd);

	return arg.retval;
}

int rd_unlink(char *pathname) {
	int fd = open("/proc/ramdisk", O_RDONLY );
	unlink_arg_t arg;
	strcpy(arg.path_name, pathname);

	ioctl(fd, RD_UNLINK, &arg);

	close(fd);

	return arg.retval;
}

int rd_chmod(char *pathname, mode_t mode) {
	int fd = open("/proc/ramdisk", O_RDONLY);
	chmod_arg_t arg;
	strcpy(arg.path_name, pathname);
	arg.mode = mode;

	ioctl(fd, RD_CHMOD, &arg);

	close(fd);

	return arg.retval;
}
