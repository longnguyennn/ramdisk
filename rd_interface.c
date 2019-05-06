#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "rd_interface.h"
#include "rd.h"

static int initialized = 0;

int rd_creat(char * pathname, mode_t mode) {
	int fd = open ( "/proc/ramdisk", O_RDONLY );

	if (initialized == 0)
		ioctl (fd, RD_INIT, NULL);

	creat_arg_t arg;
	strcpy(arg.path_name, pathname);
	arg.mode = mode;

	ioctl (fd, RD_CREAT, &arg);

	return arg.retval;
}
