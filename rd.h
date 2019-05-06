#include <linux/ioctl.h>

typedef struct {
	char path_name[MAX_PNAME_LENGTH];
	char mode;
} creat_arg_t;

typedef struct {
	char path_name[MAX_PNAME_LENGTH];
} mkdir_arg_t;

#define DIR_T   0
#define REG_T   1

#define R_ONLY  0
#define W_ONLY  1
#define RW      2

#define TYPE 552

// ioctl request number
#define RD_CREAT  _IOR( TYPE, 0, creat_arg_t)
#define RD_MKDIR  _IOR( TYPE, 1, mkdir_arg_t)
#define RD_OPEN   _IOR( TYPE, 2, char *, int)
#define RD_CLOSE  _IOR( TYPE, 3, int)
#define RD_READ   _IOWR(TYPE, 4, int, char *, int)
#define RD_WRITE  _IOWR(TYPE, 5, int, char *, int)
#define RD_LSEEK  _IOR( TYPE, 6, int, int) 
#define RD_UNLINK _IOR( TYPE, 7, char *)
#define RD_CHMOD  _IOWR(TYPE, 8, char *, char)
