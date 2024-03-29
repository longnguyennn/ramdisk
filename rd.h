#include <linux/types.h>

// File modes
#define RD  (S_IRUSR | S_IRGRP | S_IROTH)
#define RW  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define WR  (S_IWUSR | S_IRGRP | S_IROTH)

// ioctl operations
#define RD_CREAT  _IOR( 0, 0, char)
#define RD_MKDIR  _IOR( 0, 1, char)
#define RD_OPEN   _IOR( 0, 2, char)
#define RD_CLOSE  _IOR( 0, 3, char)
#define RD_READ   _IOWR(0, 4, char)
#define RD_WRITE  _IOWR(0, 5, char)
#define RD_LSEEK  _IOR( 0, 6, char)
#define RD_UNLINK _IOR( 0, 7, char)
#define RD_CHMOD  _IOWR(0, 8, char)

#define MAX_PATHNAME_LENGTH 200

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
	mode_t mode;
	int retval;
} creat_arg_t;

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
    int retval;
} mkdir_arg_t;

typedef struct {
	int fd;
	char * address;
	int num_bytes;
	int pid;
	int retval;
} read_arg_t;

typedef struct {
	int fd;
	char * address;
	int num_bytes;
	int pid;
	int retval;
} write_arg_t;

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
	int flags;
	int pid;
	int retval;
} open_arg_t;

typedef struct {
	int fd;
	int pid;
	int retval;
} close_arg_t;

typedef struct {
	int fd;
	int pid;
	int offset;
	int retval;
} lseek_arg_t;

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
	int retval;
} unlink_arg_t;

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
	mode_t mode;
	int retval;
} chmod_arg_t;
