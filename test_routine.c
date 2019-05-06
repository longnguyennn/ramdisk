#include <stdio.h>
#include <sys/stat.h>

#include "rd_interface.h"

int main () {
	int retval = rd_creat("/file1", 0);
	printf("retval = %d\n", retval);
	return 0;
}
