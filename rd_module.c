#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include "rd_module.h"

MODULE_LICENSE("GPL"); static superblock_t * sb_ptr;  // point to superblock
static inode_t * inode_array_ptr;  // point to the start of inode array
static bitmap_t * bitmap_ptr;  // point to the start of bitmap blocks
static void * content_block_ptr;  // point to the start of the content blocks

static inode_t * err_inode;
static dir_entry_t * err_dir_entry;

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

/*
 * find the file entry with name = file_name in dir_inode.
 * if found, return the directory entry of corresponding file and set flag = 1
 * if not found, set flag = 0
 * return the pointer to the next available entry if it is within allocated block
 * return err_dir_entry otherwise
 */
dir_entry_t * find_file_entry_in_dir(inode_t * dir_inode, char * file_name, int * flag) {

	int checked_entries = 0;

	/* first walk through direct block ptrs */
	for (int block_ptr = 0; block_ptr < NUM_DIRECT_BLOCK_PTR; block_ptr++) {
		/* check every entry in the block */
		for (int entry = 0; entry < NUM_ENTRIES_PER_BLOCK; entry ++) {

			// checked all of the entries in this directory and couldn't find file
			if (checked_entries * sizeof(dir_entry_t) == dir_inode->size) {
				* flag = 0;
				// current block is full
				if (entry == NUM_ENTRIES_PER_BLOCK - 1) {
					return err_dir_entry;
				}

				return (dir_entry_t *) dir_inode->location[block_ptr] + (entry + 1);
			}

			dir_entry_t * curr_entry = (dir_entry_t *) dir_inode->location[block_ptr] + entry;

			// found an entry with name == file_name
			if ( strcmp( curr_entry->fname, file_name ) == 0 ) {
				* flag = 1;
				return curr_entry;
			}

			checked_entries ++;

		}
	}

	/* TODO: walk through single/double indirect block pointer... */
	return err_dir_entry;
}

/*
 * traverse path_name to find and return the parent's inode of corresponding file.
 * path_name will be set to the corresponding file's name when func return.
 * ASSUMPTION: the path must always be valid, the file may or may not be created.
 * i.e.: path=/root/work/file.c
 * 		- /root/work must already be in the filesystem.
 * 		- file.c may or may not be created
 */
inode_t * traverse(char * path_name) {

	if (* path_name == '\0')
		return err_inode;

	char * file_name;
	inode_t * curr_inode = inode_array_ptr;  // start with the root dir
	inode_t * prev_inode;
	dir_entry_t * entry;

	while ( (file_name = strsep(&path_name, "/")) != NULL ) {
		int flag = 0;
		entry = find_file_entry_in_dir(curr_inode, file_name, &flag);

		/* there is no file with name = file_name in directory */
		if (flag == 0) {
			break;
		}

		prev_inode = curr_inode;
		curr_inode = inode_array_ptr + entry->inode_number;
	}

	strcpy(path_name, entry->fname);

	// if there is a file with the same name
	if (file_name == NULL)
		return prev_inode;

	return curr_inode;

}

/*
 * create a regular file with name = file_name and mode = mode in the parent dir.
 * if there is a file with the same name, return 1.
 * if there is no more content block/ inode left, return 1.
 * if create successfully, return 0.
 */
int create_reg_file ( inode_t * parent_inode, char * file_name, char mode ) {

	int flag = 0;
	dir_entry_t * entry = find_file_entry_in_dir(parent_inode, file_name, &flag);

	/* file with the same name already exist. */
	if (flag == 1)
		return 1;

	/* allocated blocks are full -> allocate new block to store new entry */
	if (entry == err_dir_entry) {

	}

	strcpy(entry->fname, file_name);
	entry->inode_number = inode_number;
	return 0;
}



/***
 * ioctl() entry point...
 */
static int rd_ioctl (struct inode * inode, struct file * file,
		unsigned int cmd, unsigned long arg)
{
	switch (cmd) {

		case INIT:
			/* allocate memory */
			void * memory = vmalloc (MEM_SIZE);

			/* initialize superblock */
			sb_ptr = (superblock_t *) memory;
			sb_ptr->num_free_blocks = MAX_NUM_AVAILABLE_BLOCK;
			sb_ptr->num_free_inodes = NUM_INODE;

			inode_array_ptr = (inode_t *) (sb_ptr + 1);
			bitmap_ptr = (bitmap_t *) (inode_array_ptr + NUM_INODE);
			content_block_ptr = (void *) (bitmap_ptr + 1);

			/* initialize root dir */
			inode_t * root_inode = inode_array_ptr;
			root_inode->type = DIR_T;
			root_inode->size = 0;  // currently empty
			root_inode->location[0] = content_block_ptr;  // first content block
			root_inode->access_right = RW;

			bitmap_ptr->array[0] = 8;  // mark the first content block as used (8 = 0x1000);

			sb_ptr->num_free_blocks -= 1;
			sb_ptr->num_free_inodes -= 1;

		case CREAT:
			/* get input from user space */
			creat_arg_t creat_arg;
			copy_from_user(&creat_arg, (creat_arg_t *) arg, sizeof(creat_arg_t));

			/* find the parent dir inode of given path_name */
			char * path_name = &creat_arg->path_name[1];  // ignore the leading '/' from path_name input
			inode_t * parent_inode = traverse(path_name);

			char * file_name = path_name;

			int create_status = create_reg_file(parent_inode, file_name, creat_arg->mode);

			// TODO: ERROR CHECKING with create_status...

	}
}
