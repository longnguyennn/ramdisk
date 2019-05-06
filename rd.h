#define READ_ONLY  0
#define WRITE_ONLY 1
#define READ_WRITE 2

// ioctl operations
#define RD_INIT   _IOR( 0, 0, int )
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
	char mode;
	int retval;
} creat_arg_t;

typedef struct {
	char path_name[MAX_PATHNAME_LENGTH];
} mkdir_arg_t;
