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
MODULE_VERSION("1.0");

long int counter;

static void* procfile_start(struct seq_file* seq, loff_t* pos)
{
    struct list_head* data;
    printk(KERN_INFO "Entering procfile_start with pos==%lld\n", *pos);
    data = seq_list_start(&data_list, *pos);
    if (data == NULL) {
        *pos = 0;
    }
    return data;
    // if (*pos == 0) {
    //     // If we are at the start of the file, return &counter
    //     counter = 0;
    //     return &counter;
    // } else if (*pos < 10) {
    //     // We are in the file, keep going
    //     return &counter;
    // } else {
    //     // We are at the end of the file, stop
    //     *pos = 0;
    //     return NULL;
    // }
}
static void* procfile_next(struct seq_file* seq, void* v, loff_t* pos)
{
    // Iterate to the next element in the linked list
    printk(KERN_INFO "Entering procfile_next\n");
    return seq_list_next(v, &data_list, pos);
    // data_t* data;
    // struct list_head* node;
    // node = seq_list_next(v, &data_list, pos);
    // data = list_entry(node, data_t, list);
    // seq_printf(seq, "Extracted one data fragment\n");
    // data = list_next_entry(data, list);
    // return NULL;
    // long int* tmp = (long int*)v;
    // printk(KERN_INFO "Entering procfile_next with pos=%lld\n", *pos);
    // ++(*tmp);
    // ++(*pos);
    // if (*pos == 10) {
    //     printk(KERN_INFO "Stopping due to *pos==%d\n", 10);
    //     return NULL;
    // } else {
    //     printk(KERN_INFO "Proceeding\n");
    //     return tmp;
    // }
}

static void procfile_stop(struct seq_file* seq, void* v)
{
    // Nothing to do here
    printk(KERN_INFO "Entering procfile_stop\n");
    return;
}

static int procfile_show(struct seq_file* seq, void* v)
{
    data_t* tmp;
    printk(KERN_INFO "Full show has been called\n");
    tmp = list_entry(v, data_t, list);
    seq_printf(seq, "Extracted one data fragment: %d\n", tmp->nb_cpus);
    return 0;
}

static int single_show(struct seq_file* seq, void* v)
{
    data_t* data;
    data = kmalloc(sizeof(data_t), GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "Failed to allocate memory for the data\n");
        return -1;
    }
    INIT_LIST_HEAD(&data->list);
    list_add(&data->list, &data_list);
    data_list_len++;
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Buffer size: %ld\n", seq->size);
    // Print a header
    seq_printf(seq, "Welcome to Sentinel\n");
    // Get the data
    populate_data(data);
    // Print the data
    print_data(seq, *data);
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
    proc_file_full = proc_create(procfs_name_full, 0444, NULL, &f_ops_full);
    data_list_len = 0;
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