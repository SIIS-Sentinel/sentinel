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
#include <linux/time.h>

#include "sentinel.h"
#include "sentinel_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("1.0");

static int single_show(struct seq_file* seq, void* v)
{
    data_t data;
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Buffer size: %ld\n", seq->size);
    // Print a header
    seq_printf(seq, "Welcome to Sentinel\n");
    // Get the data
    populate_data(&data);
    // Print the data
    print_data(seq, data);
    return 0;
}

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open(struct inode* inode, struct file* file)
{
    return single_open(file, single_show, NULL);
}

static int __init hello(void)
{
    printk(KERN_INFO "Hello world\n");
    proc_dir = proc_mkdir(procfs_dir_name, NULL);
    proc_file_single = proc_create(procfs_name_single, 0444, proc_dir, &f_ops);
    return 0;
}

static void __exit goodbye(void)
{
    proc_remove(proc_file_single);
    proc_remove(proc_dir);
    printk(KERN_INFO "Goodbye world\n");
}

module_init(hello);
module_exit(goodbye);