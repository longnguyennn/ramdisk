#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include "rd_interface.h"
#include "rd.h"

static int initialized = 0;

int rd_creat(char * pathname, mode_t mode) {
	int fd = open ( "/proc/ramdisk", O_RDONLY );


	if (initialized == 0) {
		initialized ++;
		void * ptr = malloc(sizeof(void *));
		ioctl (fd, RD_INIT, ptr);
		free(ptr);
	}

//	creat_arg_t arg;
//	strcpy(arg.path_name, pathname);
//	arg.mode = mode;
//
//	ioctl (fd, RD_CREAT, &arg);

//	return arg.retval;

	return 1;
}
