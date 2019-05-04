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

/* ioctl operation */
#define MALLOC _IOW(0, 1, char)

typedef struct {
	unsigned long num_free_blocks;
	unsigned long num_free_inodes;
}__attribute__((alligned(128))) superblock_t;  // 128 byte alignment to make sizeof() = 256

/* sizeof(inode_t) = 64 already, doesn't need to specify alignment */
typedef struct {
	char type;  // 0 = dir / 1 = reg
	unsigned long size;
	void * location[10];
	char access_right;  // 0 = r, 1 = w, 2 = rw
} inode_t;

static superblock_t * sb_ptr;
static inode_t * inode_array_ptr;  // point to the start of inode array

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
		case MALLOC:

	}
}
