#include <stdio.h>
#include <sys/stat.h>

#include "rd_interface.h"

#define MAX_FILES 1023
#define RD  (S_IRUSR | S_IRGRP | S_IROTH)
#define RW  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define WR  (S_IWUSR | S_IRGRP | S_IROTH)

int main () {

	int retval, i;
	char pathname[80];

	// for (i = 0; i < MAX_FILES; i ++) {
	// 	sprintf(pathname, "/file%d", i);
	// 	retval = rd_creat(pathname, 0);

	// 	if (retval < 0)
	// 		printf("ERROR! status: %d (%s)\n", retval, pathname);
	// }

	char *pathname1 = "/hi";
	char *pathname2 = "/hi/fi2";
	retval = rd_mkdir(pathname1);
	printf("Ret status: %d (%s)\n", retval, pathname1);

	retval = rd_creat(pathname2, RW );
	printf("Ret status: %d (%s)\n", retval, pathname2);

	retval = rd_open(pathname2, 2 );
	printf("Ret status: %d (%s)\n", retval, pathname2);

	return 0;
}
