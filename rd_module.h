#include <linux/types.h>

/* ramdisk constants */
#define MEM_SIZE 2097152  // 2MB
#define _BLOCK_SIZE 256
#define NUM_INODE 1024  // 256 blocks of inode, block size = 256 bytes, inode size = 64 byte
#define MAX_NUM_AVAILABLE_BLOCK 7931  // 2MB = 8192 * (256 byte) blocks - (superblock + inode blocks + bitmap blocks)
#define NUM_DIRECT_BLOCK_PTR 8
#define NUM_ENTRIES_PER_BLOCK 16
#define BITMAP_ARR_LENGTH 1024
#define INODE_BITMAP_LENGTH 240
#define MAX_NUM_PROCESS 2
#define MAX_OPEN_FILE 50

#define FILE_UNINITIALIZED -2
#define PROC_UNINITIALIZED -2

#define DIR_T 0
#define REG_T 1

/* sizeof(superblock_t) = sizeof(unsigned long) * 2 + 248 = 256 */
typedef struct {
	unsigned long num_free_blocks;
	unsigned long num_free_inodes;
	char inode_bitmap[INODE_BITMAP_LENGTH];  // keep track of available inode
	int process_table[MAX_NUM_PROCESS];  // store the PID of running processes
} superblock_t;

/* sizeof(inode_t) = 64 */
typedef struct {
	char type;
	unsigned long size;
	mode_t access_right;
	void * location[10];
}__attribute__((aligned(16))) inode_t;

/* sizeof(bitmap_t) = 1024 byte = 4 * (256 byte blocks) */
typedef struct {
	char array[BITMAP_ARR_LENGTH];
} bitmap_t;

/* size of (dir_entry_t) = 16 */
typedef struct {
	char fname[14];
	short inode_number;
} dir_entry_t;

typedef struct {
	int position;
	inode_t * inode_ptr;
} file_t;

/* function headers */
void rd_init(void);
dir_entry_t * find_file_entry_in_dir(inode_t *, char *, int *);
inode_t * traverse(char *, char *);
int create_reg_file(inode_t *, char *, mode_t);
void * get_available_block(void);
int get_available_inode_idx(void);
inode_t * find_inode(char * path_name);
int check_file_permission(int, mode_t);
