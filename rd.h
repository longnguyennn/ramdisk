#include <linux/ioctl.h>

#define READ_WRITE 'r'
#define READ_ONLY  'o'
#define WRITE_ONLY 'w'

#define TYPE 552

// ioctl request number
#define RD_CREAT  _IOR( TYPE, 0, char *, char)
#define RD_MKDIR  _IOR( TYPE, 1, char *, char)
#define RD_OPEN   _IOR( TYPE, 2, char *, int)
#define RD_CLOSE  _IOR( TYPE, 3, int)
#define RD_READ   _IOWR(TYPE, 4, int, char *, int)
#define RD_WRITE  _IOWR(TYPE, 5, int, char *, int)
#define RD_LSEEK  _IOR( TYPE, 6, int, int) 
#define RD_UNLINK _IOR( TYPE, 7, char *)
#define RD_CHMOD  _IOWR(TYPE, 8, char *, char)
