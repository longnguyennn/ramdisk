/* ramdisk constants */
#define MEM_SIZE 2097152  // 2MB
#define BLOCK_SIZE 256
#define NUM_INODE 1024  // 256 blocks of inode, block size = 256 bytes, inode size = 64 byte
#define MAX_NUM_AVAILABLE_BLOCK 7931  // 2MB = 8192 * (256 byte) blocks - (superblock + inode blocks + bitmap blocks)
#define MAX_PNAME_LENGTH 200
#define NUM_DIRECT_BLOCK_PTR 8
#define NUM_ENTRIES_PER_BLOCK 16
#define BITMAP_ARR_LENGTH 1024
#define INODE_BITMAP_LENGTH 240

#define DIR_T 0
#define REG_T 1
#define R_ONLY 0
#define W_ONLY 1
#define RW 2

/* ioctl operations */
#define INIT _IOW(0, 1, char)
#define CREAT _IOW(0, 2, char)
#define MKDIR _IOW(0, 3, char)

/* sizeof(superblock_t) = sizeof(unsigned long) * 2 + 240 = 256 */
typedef struct {
	unsigned long num_free_blocks;
	unsigned long num_free_inodes;
	char inode_bitmap[INODE_BITMAP_LENGTH];  // keep track of available inode
} superblock_t;

/* sizeof(inode_t) = 64 already, doesn't need to specify alignment */
typedef struct {
	char type;
	unsigned long size;
	void * location[10];
	char access_right;
} inode_t;

/* sizeof(bitmap_t) = 1024 byte = 4 * (256 byte blocks) */
typedef struct {
	char array[BITMAP_ARR_LENGTH];
} bitmap_t;

typedef struct {
	char fname[14];
	int inode_number;
} dir_entry_t;

typedef struct {
	char path_name[MAX_PNAME_LENGTH];
	char mode;
} creat_arg_t;

typedef struct {
	char path_name[MAX_PNAME_LENGTH];
} mkdir_arg_t;

/* function headers */
dir_entry_t * find_file_entry_in_dir(inode_t *, char *, int *);
inode_t * traverse(char *);
int create_reg_file(inode_t *, char *, char);
void * get_available_block();
int get_available_inode_idx();
