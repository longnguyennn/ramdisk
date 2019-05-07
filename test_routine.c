#include <stdio.h>
#include <sys/stat.h>

#include "rd_interface.h"

#define MAX_FILES 1023

int main () {

	int retval, i;
	char pathname[80];

	for (i = 0; i < MAX_FILES; i ++) {
		sprintf(pathname, "/file%d", i);
		retval = rd_creat(pathname, 0);

		if (retval < 0)
			printf("ERROR! status: %d (%s)\n", retval, pathname);
	}

	printf("SUCCESS\n");


	return 0;
}
