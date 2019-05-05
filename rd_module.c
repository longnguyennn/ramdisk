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
 * traverse path_name to find and return the parent's inode of corresponding file.
 * path_name will be set to the corresponding file's name when func return.
 */
inode_t * traverse(char * path_name) {

	if (* path_name == '\0')
		return err_inode;

	char * curr_file_name;

	char * dir_name;
	inode_t * curr_inode = inode_array_ptr;  // start with the root dir

	while ( (dir_name = strsep(&path_name, "/")) != NULL ) {
		/* find the dir_entry with name = dir_name inside curr_inode */
		int checked_entries = 0;
		int found_entry = 0;

		// first walk through direct block pointers to find entry dir_name
		for (int i = 0; i < NUM_DIRECT_BLOCK_PTR; i ++)
		{
			// each block can have maximum 256 bytes / 16 bytes per entry = 16 entries
			for (int j = 0; j < NUM_ENTRIES_PER_BLOCK; j ++) {

				// checked all of the entries in current directory
				if (checked_entries * sizeof(dir_entry_t) == curr_inode->size)
					return err_inode;

				dir_entry_t * curr_dir_entry = ( (dir_entry_t *) curr_inode->location[i] ) + j;

				// found an entry with fname == dir_name
				if ( strcmp(curr_dir_entry->fname, dir_name) == 0 ) {
					curr_inode = inode_array_ptr + curr_dir_entry->inode_number;
					found_entry = 1;
					break;
				}
				checked_entries ++;
			}

			// found the entry in one of the direct block ptr locations
			if (found_entry == 1) {
				break;
			}
		}

		// NOTE: walk through single/double indirect block pointer here...
		//if (found_entry == 0) {
			// walk through single-indirect block pointer
			// walk thorugh double-indirect block pointer
		//}

		// save dir_name to curr_file_name
		strcpy(curr_file_name, dir_name);
	}

	strcpy(path_name, curr_file_name);  // set path_name to file's name before return
	return curr_inode;

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

			// cannot find parent_inode
			// if (parent_inode == err_inode)...

			char * file_name = path_name;

			create_entry(parent_inode, file_name);

	}
}
