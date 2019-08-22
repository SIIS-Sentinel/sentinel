#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h>

#include "sentinel_helper.h"
#include "sentinel_procfs.h"

// Seq file ops
struct seq_operations seq_ops = {
    .start = procfile_start,
    .next = procfile_next,
    .stop = procfile_stop,
    .show = procfile_show
};

struct file_operations f_ops_full = {
    .owner = THIS_MODULE,
    .open = procfs_open_full,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release
};

struct file_operations f_ops_single = {
    .owner = THIS_MODULE,
    .open = procfs_open_single,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release
};

struct proc_dir_entry* proc_dir;
struct proc_dir_entry* proc_file_single;
struct proc_dir_entry* proc_file_full;
uint32_t data_list_len;
struct list_head* buf;
LIST_HEAD(data_list);
struct timer_list data_timer;
int list_len;

void* procfile_start(struct seq_file* seq, loff_t* pos)
{
    struct list_head* data;
    printk(KERN_INFO "Entering procfile_start with pos==%lld\n", *pos);
    data = seq_list_start_head(&data_list, *pos);
    if (data == NULL) {
        *pos = 0;
        free_list(&data_list);
    }
    return data;
}

void* procfile_next(struct seq_file* seq, void* v, loff_t* pos)
{
    // Iterate to the next element in the linked list
    printk(KERN_INFO "Entering procfile_next\n");
    return seq_list_next(v, &data_list, pos);
}

void procfile_stop(struct seq_file* seq, void* v)
{
    // Nothing to do here
    printk(KERN_INFO "Entering procfile_stop\n");
    return;
}

int procfile_show(struct seq_file* seq, void* v)
{
    printk(KERN_INFO "Full show has been called\n");
    print_data_short(seq, &data_list, v);
    return 0;
}

int single_show(struct seq_file* seq, void* v)
{
    data_t* data;
    data = create_data_node(&data_list, &list_len, GFP_KERNEL);
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
int procfs_open_full(struct inode* inode, struct file* file)
{
    return seq_open(file, &seq_ops);
}

// Opens the seq file for future operations, and returns a file descriptor to it
int procfs_open_single(struct inode* inode, struct file* file)
{
    return single_open(file, single_show, NULL);
}

void timer_handler(struct timer_list* timer)
{
    data_t* data;
    printk(KERN_INFO "Sentinel timer handler called\n");
    // Reset the timer
    timer->expires = jiffies + (TIMER_DELAY * HZ);
    add_timer(timer);
    // Add a new data node
    data = create_data_node(&data_list, &list_len, GFP_ATOMIC);
    populate_data(data);
    return;
}