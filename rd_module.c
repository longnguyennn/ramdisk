#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/types.h>

#include "rd_module.h"
#include "rd.h"

MODULE_LICENSE("GPL"); static superblock_t * sb_ptr;  // point to superblock

static void * memory;
static inode_t * inode_array_ptr;  // point to the start of inode array
static bitmap_t * bitmap_ptr;  // point to the start of bitmap blocks
static void * content_block_ptr;  // point to the start of the content blocks

static inode_t err_inode;
static dir_entry_t err_dir_entry;

static int rd_ioctl (struct inode * inode, struct file * file,
		unsigned int cmd, unsigned long arg);

static struct file_operations rd_proc_operations;

static struct proc_dir_entry * proc_entry;

static int __init initialization_routine (void) {
	printk("Loading module.\n");

	rd_proc_operations.ioctl = rd_ioctl;

	/* Create proc entry */
	proc_entry = create_proc_entry("ramdisk", 0444, NULL);
	if (!proc_entry)
	{
		printk("Error creating /proc entry.\n");
		return 1;
	}

	proc_entry->proc_fops = &rd_proc_operations;

	return 0;
}

static void __exit cleanup_routine (void) {
	printk("Dumping module.\n");
	remove_proc_entry("ramdisk", NULL);
	vfree(memory);
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
	int block_ptr;
	for (block_ptr = 0; block_ptr < NUM_DIRECT_BLOCK_PTR; block_ptr++) {
		/* check every entry in the block */
		int entry;
		for (entry = 0; entry < NUM_ENTRIES_PER_BLOCK; entry ++) {

			// checked all of the entries in this directory and couldn't find file
			if (checked_entries * sizeof(dir_entry_t) == dir_inode->size) {
				* flag = 0;
				// allocated blocks are full
				if (entry == 0) {
					return &err_dir_entry;
				}

				return (dir_entry_t *) dir_inode->location[block_ptr] + entry;
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
	return &err_dir_entry;
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

	char * file_name;
	dir_entry_t * entry;
	inode_t * prev_inode;
	inode_t * curr_inode = inode_array_ptr;

	// this should never be the case
	if (* path_name == '\0')
		return &err_inode;

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

	// if there is a file with the same name
	if (file_name == NULL) {
		strcpy(path_name, entry->fname);
		return prev_inode;
	}

	strcpy(path_name, file_name);
	return curr_inode;

}

/*
 * create a regular file with name = file_name and mode = mode in the parent dir.
 * if there is a file with the same name, return -1.
 * if there is no more content block/ inode left, return -1.
 * if create successfully, return 0.
 */
int create_reg_file ( inode_t * parent_inode, char * file_name, mode_t mode ) {

	/* no inode available */
	if (sb_ptr->num_free_inodes == 0)
		return -1;

	int flag = 0;
	dir_entry_t * entry = find_file_entry_in_dir(parent_inode, file_name, &flag);

	/* file with the same name already exist. */
	if (flag == 1)
		return -1;

	/* blocks allocated for parent directory are full -> allocate new block to store new entry */
	if (entry == &err_dir_entry) {

		/* there is no block available */
		if (sb_ptr->num_free_blocks == 0)
		   return -1;

		/* allocate new direct block pointer */
		if (parent_inode->size < NUM_DIRECT_BLOCK_PTR * _BLOCK_SIZE) {
			int next_block_ptr = parent_inode->size / _BLOCK_SIZE;
			parent_inode->location[next_block_ptr] = get_available_block();
			entry = (dir_entry_t *) parent_inode->location[next_block_ptr];
		}
		/* TODO: allocate new single/double indirect block pointer */


		/* update superblock */
		sb_ptr->num_free_blocks --;

	}

	/* create a new inode */
	int inode_idx = get_available_inode_idx();
	inode_t * inode = inode_array_ptr + inode_idx;

	inode->type = REG_T;
	inode->size = 0;
	inode->access_right = mode;

	/* write a directory entry to the location * entry is pointing to */
	strcpy(entry->fname, file_name);
	entry->inode_number = inode_idx;

	/* update parent inode */
	parent_inode->size += sizeof(dir_entry_t);

	/* update superblock */
	sb_ptr->num_free_inodes --;

	return 0;
}

/* return the address of an available block and mark that block as used in the bitmap.
 * this func should be called after checking sb_ptr->num_free_blocks to avoid potential misbehavior.
 */
void * get_available_block(void) {

	/* walk through the bitmap and check each bit. */
	int curr_block = 0;  // index into content block array
	int i;
	for (i = 0; i < BITMAP_ARR_LENGTH; i ++) {
		char byte = bitmap_ptr->array[i];

		/* first bit is not set */
		if (byte < 128) {
			bitmap_ptr->array[i] += 128;  // set the 1st bit
			break;
		}
		curr_block ++;

		/* second bit is not set */
		if (byte < 192) {
			bitmap_ptr->array[i] += 64;  // set the 2nd bit
			break;
		}
		curr_block ++;

		/* ... bit is not set */
		if (byte < 224) {
			bitmap_ptr->array[i] += 32;  // set the 3rd bit
			break;
		}
		curr_block ++;

		if (byte < 240) {
			bitmap_ptr->array[i] += 16;  // ...
			break;
		}
		curr_block ++;

		if (byte < 248) {
			bitmap_ptr->array[i] += 8;
			break;
		}
		curr_block ++;

		if (byte < 252) {
			bitmap_ptr->array[i] += 4;
			break;
		}
		curr_block ++;

		if (byte < 254) {
			bitmap_ptr->array[i] += 2;
			break;
		}
		curr_block ++;

		if (byte < 255) {
			bitmap_ptr->array[i] += 1;
			break;
		}
		curr_block ++;

	}

	return (superblock_t *) content_block_ptr + curr_block;  // start of the content block addr + block_size * curr_block
}

/* return the idx into the inode array of an available inode and mark it as used in bitmap.
 * this func should be called after checking sb_ptr->num_free_blocks to avoid potential misbehavior.
 */
int get_available_inode_idx(void) {

	/* walk through the bitmap and check each bit. */
	int idx = 0;
	int i;
	for (i = 0; i < INODE_BITMAP_LENGTH; i ++ ) {
		char byte = sb_ptr->inode_bitmap[i];

		/* first bit is not set */
		if (byte < 128) {
			sb_ptr->inode_bitmap[i] += 128;
			break;
		}
		idx ++;

		/* second bit is not set */
		if (byte < 192) {
			sb_ptr->inode_bitmap[i] += 64;
			break;
		}
		idx ++;

		/* third bit is not set */
		if (byte < 224) {
			sb_ptr->inode_bitmap[i] += 32;
			break;
		}
		idx ++;

		if (byte < 240) {
			sb_ptr->inode_bitmap[i] += 16;
			break;
		}
		idx ++;

		if (byte < 248) {
			sb_ptr->inode_bitmap[i] += 8;
			break;
		}
		idx ++;

		if (byte < 252) {
			sb_ptr->inode_bitmap[i] += 4;
			break;
		}
		idx ++;

		if (byte < 254) {
			sb_ptr->inode_bitmap[i] += 2;
			break;
		}
		idx ++;

		if (byte < 255) {
			sb_ptr->inode_bitmap[i] += 1;
			break;
		}
		idx ++;
	}

	return idx;
}



/***
 * ioctl() entry point...
 */
static int rd_ioctl (struct inode * inode, struct file * file,
		unsigned int cmd, unsigned long arg)
{
	switch (cmd) {

		case RD_INIT: ;
			/* allocate memory */
			memory = vmalloc (MEM_SIZE);

			/* initialize superblock */
			sb_ptr = (superblock_t *) memory;
			sb_ptr->num_free_blocks = MAX_NUM_AVAILABLE_BLOCK;
			sb_ptr->num_free_inodes = NUM_INODE;

			inode_array_ptr = (inode_t *) (sb_ptr + 1);
			bitmap_ptr = (bitmap_t *) (inode_array_ptr + NUM_INODE);
			content_block_ptr = (void *) (bitmap_ptr + 1);

			/* initialize every bit in bitmap to 0 */
			memset((void *) bitmap_ptr, 0, sizeof(bitmap_t));
			memset(&sb_ptr->inode_bitmap, 0, INODE_BITMAP_LENGTH);

			/* initialize root dir */
			inode_t * root_inode = inode_array_ptr;
			root_inode->type = DIR_T;
			root_inode->size = 0;  // currently empty
			root_inode->location[0] = content_block_ptr;  // first content block
			root_inode->access_right = RW;

			sb_ptr->inode_bitmap[0] = 128;  // mark the first inode as used (128 = 0x1000000)
			sb_ptr->num_free_inodes -= 1;

			//bitmap_ptr->array[0] = 128;  // mark the first content block as used (128 = 0x10000000)
			//sb_ptr->num_free_blocks -= 1;
			//

			printk("superblock_t = %d\n", sizeof(superblock_t));
			printk("inode_t = %d\n", sizeof(inode_t));
			printk("bitmap_ptr = %d\n", sizeof(bitmap_t));
			printk("dir_entry_t = %d\n", sizeof(dir_entry_t));

			printk("Layout addr = ... \n");

			printk("0x%p\n", sb_ptr);
			printk("0x%p\n", inode_array_ptr);
			printk("0x%p\n", bitmap_ptr);
			printk("0x%p\n", content_block_ptr);

			break;

		case RD_CREAT: ;
			/* get input from user space */
			creat_arg_t creat_arg;
			copy_from_user(&creat_arg, (creat_arg_t *) arg, sizeof(creat_arg_t));

			/* find the parent dir inode of given path_name */
			char * path_name = &creat_arg.path_name[1];  // ignore the leading '/' from path_name input
			inode_t * parent_inode = traverse(path_name);

			char * file_name = path_name;

			int create_status = create_reg_file(parent_inode, file_name, creat_arg.mode);

			copy_to_user((int *) & ( (creat_arg_t *) arg ) -> retval, &create_status, sizeof(int));

			break;

		default:
			return -EINVAL;
			break;

	}

	return 0;
}

module_init(initialization_routine);
module_exit(cleanup_routine);
