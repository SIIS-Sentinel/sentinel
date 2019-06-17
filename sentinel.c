#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>

#include "sentinel.h"
#include "sentinel_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("2.0");

static void* procfile_start(struct seq_file* seq, loff_t* pos)
{
    struct list_head* data;
    printk(KERN_INFO "Entering procfile_start with pos==%lld\n", *pos);
    data = seq_list_start_head(&data_list, *pos);
    if (data == NULL) {
        *pos = 0;
    }
    return data;
}
static void* procfile_next(struct seq_file* seq, void* v, loff_t* pos)
{
    // Iterate to the next element in the linked list
    printk(KERN_INFO "Entering procfile_next\n");
    return seq_list_next(v, &data_list, pos);
}

static void procfile_stop(struct seq_file* seq, void* v)
{
    // Nothing to do here
    printk(KERN_INFO "Entering procfile_stop\n");
    return;
}

static int procfile_show(struct seq_file* seq, void* v)
{
    printk(KERN_INFO "Full show has been called\n");
    print_data_short(seq, &data_list, v);
    return 0;
}

static int single_show(struct seq_file* seq, void* v)
{
    data_t* data;
    data = create_data_node(&data_list);
    if (!data) {
        printk(KERN_ERR "Failed to allocate memory for the data\n");
        return -1;
    }
    printk(KERN_INFO "Read handler of Sentinel called\n");
    // Print a header
    seq_printf(seq, "Welcome to Sentinel\n");
    // Get the data
    populate_data(data);
    // Print the data
    print_data_verbose(seq, *data);
    return 0;
}

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open_full(struct inode* inode, struct file* file)
{
    return seq_open(file, &seq_ops);
}

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open_single(struct inode* inode, struct file* file)
{
    return single_open(file, single_show, NULL);
}

static int __init hello(void)
{
    printk(KERN_INFO "Hello world\n");
    proc_dir = proc_mkdir(procfs_dir_name, NULL);
    proc_file_single = proc_create(procfs_name_single, 0444, proc_dir, &f_ops_single);
    proc_file_full = proc_create(procfs_name_full, 0444, proc_dir, &f_ops_full);
    return 0;
}

static void __exit goodbye(void)
{
    data_t *pos, *next;
    list_for_each_entry_safe(pos, next, &data_list, list)
    {
        list_del(&pos->list);
        kfree(pos);
    }
    proc_remove(proc_file_single);
    proc_remove(proc_file_full);
    proc_remove(proc_dir);
    printk(KERN_INFO "Goodbye world\n");
}

module_init(hello);
module_exit(goodbye);