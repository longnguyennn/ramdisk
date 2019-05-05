#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/tty.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

/* ramdisk constants */
#define MEM_SIZE 2097152  // 2MB
#define NUM_INODE 1024  // 256 blocks of inode, block size = 256 bytes, inode size = 64 byte
#define MAX_NUM_AVAILABLE_BLOCK 7931  // 2MB = 8192 * (256 byte) blocks - (superblock + inode blocks + bitmap blocks)

#define DIR_T 0
#define REG_T 1
#define R_ONLY 0
#define W_ONLY 1
#define RW 2

/* ioctl operation */
#define INIT _IOW(0, 1, char)
#define CREAT(0, 2, char)
#define MKDIR(0, 3, char)

typedef struct {
	unsigned long num_free_blocks;
	unsigned long num_free_inodes;
}__attribute__((alligned(128))) superblock_t;  // 128 byte alignment to make sizeof() = 256

/* sizeof(inode_t) = 64 already, doesn't need to specify alignment */
typedef struct {
	char type;
	unsigned long size;
	void * location[10];
	char access_right;
} inode_t;

/* sizeof(bitmap_t) = 1024 byte = 4 * (256 byte blocks) */
typedef struct {
	char array[1024];
} bitmap_t;

typedef struct {
	char fname[14];
	int inode_number;
} dir_entry_t;

static superblock_t * sb_ptr;  // point to superblock
static inode_t * inode_array_ptr;  // point to the start of inode array
static bitmap_t * bitmap_ptr;  // point to the start of bitmap blocks
static void * content_block_ptr;  // point to the start of the content blocks

static int rd_ioctl (struct inode * inode, struct file * file,
		unsigned int cmd, unsigned long arg);

static struct file_operations rd_proc_operations;

static struct proc_dir_entry * proc_entry;

static int __init initialization_routine (void) {
	printk("<rd> Loading module.\n");

	rd_proc_operations.ioctl = rd_ioctl;

	/* Create proc entry */
	proc_entry = create_proc_entry("ramdisk", 0444, NULL);
	if (!proc_entry)
	{
		printk("<rd> Error creating /proc entry.\n");
		return 1;
	}

	proc_entry->proc_fops = &rd_proc_operations;

	return 0;
}

static void __exit cleanup_routine (void) {
	printk("<rd> Dumping module.\n");
	remove_proc_entry("ramdisk", NULL);
	return;
}

/* 'printk' version that prints to active tty. */
void my_printk(char *string)
{
	struct tty_struct *my_tty;

	my_tty = current->signal->tty;

	if (my_tty != NULL) {
		(*my_tty->driver->ops->write)(my_tty, string, strlen(string));
		(*my_tty->driver->ops->write)(my_tty, "\015\012", 2);
	}
}

/***
 * ioctl() entry point...
 */
static int rd_ioctl (struct inode * inode, struct file * file,
		unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		// init memory area for ramdisk
		case INIT:
			/* allocate memory */
			void * memory = vmalloc (MEM_SIZE);

			/* initialize superblock */
			sb_ptr = (superblock_t *) memory;
			sb_ptr->num_free_blocks = MAX_NUM_AVAILABLE_BLOCK;
			sb_ptr->num_free_inodes = NUM_INODE;

			inode_array_ptr = (inode_t *) (sb_ptr + 1);
			bitmap_ptr = (bitmap_t *) (inode_array_ptr + NUM_INODE);
			content_block_ptr = (bitmap_ptr + 1);

			/* initialize root dir */
			inode_t * root_inode = inode_array_ptr;
			root_inode->type = DIR_T;
			root_inode->size = 0;  // currently empty
			root_inode->location[0] = content_block_ptr;  // first content block
			root_inode->access_right = RW;

			bitmap_ptr->array[0] = 8;  // mark the first content block as used (8 = 0x1000);

			sb_ptr->num_free_blocks -= 1;
			sb_ptr->num_free_inodes -= 1;
	}
}
