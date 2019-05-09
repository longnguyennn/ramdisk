#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "rd_interface.h"
#include "rd.h"

#define MAX_FILES 1023

int main () {

	int retval, i;
	char * filepath = "/file";

	rd_creat(filepath, RW);
	int fd = rd_open(filepath, O_RDWR);

	printf("fd = %d\n", fd);

	char * write_string = "123";

	retval = rd_write(fd, write_string, 4);

	printf("write status : %d\n", retval);

	retval = rd_lseek(fd, 0);

	printf("lseek to 0 status: %d\n", retval);

	char read_buffer[10];

	retval = rd_read(fd, read_buffer, 4);

	printf("read status: %d\n", retval);
	printf("read_buffer = %s\n", read_buffer);

	retval = rd_close(fd);
	printf("close status: %d\n", retval);

	//char *pathname1 = "/hi";
	//char *pathname2 = "/hi/fi2";
	//char *pathname3 = "/ih/fi2";
	//char *pathname4 = "/hi/close";

	//printf("BASIC TEST FOR MKDIR:\n");

	//retval = rd_mkdir(pathname1);
	//printf("TEST 1: mkdir with valid pathname: 0 == %d\n", retval);

	//retval = rd_mkdir(pathname2);
	//printf("TEST 2: mkdir with nested dir: 0 == %d\n", retval);

	//retval = rd_mkdir(pathname3);
	//printf("TEST 3: mkdir with invalid pathname: -1 == %d\n", retval);

	//retval = rd_mkdir(pathname2);
	//printf("TEST 4: mkdir with already exist dir: -1 == %d\n", retval);

	//printf("BASIC TEST FOR OPEN:\n");

	//retval = rd_open(pathname1, O_RDONLY);
	//printf("TEST 1: open with valid pathname: 0 == %d\n", retval);

	//retval = rd_open(pathname1, O_RDONLY);
	//printf("TEST 2: open already opened file: -1 == %d\n", retval);

	//retval = rd_open(pathname2, O_RDONLY);
	//printf("TEST 3: open nested dir: 1 == %d\n", retval);

	//retval = rd_open(pathname3, O_RDONLY);
	//printf("TEST 4: open non-exist file: -1 == %d\n", retval);

	//char *filepath = "/file";

	//retval = rd_creat(filepath, RD);
	//if (retval < 0) {
	//	printf("rd_creat err...\n");
	//	return -1;
	//}

	//retval = rd_open(filepath, O_RDONLY);
	//printf("TEST 5: open file with file mode = RD with flag O_RDONLY: 2 == %d\n", retval);

	//retval = rd_open(filepath, O_WRONLY);
	//printf("TEST 5: open file with file mode = RD with flag O_WRONLY: -1 == %d\n", retval);

	//retval = rd_open(filepath, O_RDWR);
	//printf("TEST 6: open file with file mode = RD with flag O_RDWR: -1 == %d\n", retval);

	//filepath = "/file1";

	//retval = rd_creat(filepath, WR);
	//if (retval < 0) {
	//	printf("rd_creat err...\n");
	//	return -1;
	//}

	//retval = rd_open(filepath, O_RDONLY);
	//printf("TEST 7: open file with file mode = WR with flag O_RDONLY: -1 == %d\n", retval);

	//retval = rd_open(filepath, O_WRONLY);
	//printf("TEST 8: open file with file mode = WR with flag O_WRONLY: 3 == %d\n", retval);

	//retval = rd_open(filepath, O_RDWR);
	//printf("TEST 9: open file with file mode = WR with flag O_RDWR: -1 == %d\n", retval);

	//filepath = "/newfile";
	//retval = rd_creat(filepath, RW);
	//if (retval < 0) {
	//	printf("rd_creat err...\n");
	//	return -1;
	//}

	//printf("TEST 10: open same file in different process\n");
	//if ((retval = fork())) {
	//	retval = rd_open(filepath, O_RDONLY);
	//	printf("(Parent): %d\n", retval);
	//}
	//else {
	//	retval = rd_open(filepath, O_RDONLY);
	//	printf("(Child): %d\n", retval);
	//}

	//printf("BASIC TEST FOR CLOSE:\n");

	// retval = rd_creat(pathname4, RW);
	// printf("Creating %s: 0 = %i\n", pathname4, retval);

	// int close_fd = rd_open(pathname4, O_RDONLY); // the fd
	// printf("Openning %s: fd >= 0 = %i\n", pathname4, close_fd);

	//retval = rd_creat(pathname4, RW);
	//int close_fd = rd_open(pathname4, O_RDONLY); // the fd

	//retval = rd_close(close_fd);
	//printf("TEST 1: close a valid file: 0 == %d\n", retval);

	//retval = rd_close(close_fd);
	//printf("TEST 2: close a already closed file: -1 == %d\n", retval);

	// printf("BASIC TEST FOR UNLINK:\n");

	// retval = rd_unlink(pathname4);
	// printf("TEST 1: unlink a valid pathname: 0 == %d\n", retval);

	return 0;
}
