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
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "sentinel.h"
#include "sentinel_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("2.0");

struct kobject* sysfs_entry;
struct attribute attr;

static ssize_t sentinel_sysfs_show(struct kobject* kobj, struct attribute* attr, char* buf)
{
    printk(KERN_INFO "sysfs show called\n");
    return sprintf(buf, "1\n");
}

static ssize_t sentinel_sysfs_store(struct kobject* kobj, struct attribute* attr, const char* buf, size_t len)
{
    printk(KERN_INFO "sysfs show called\n");
    return -EIO;
}

static const struct sysfs_ops sentinel_sysops = {
    .show = sentinel_sysfs_show,
    .store = sentinel_sysfs_store,
};

static void* procfile_start(struct seq_file* seq, loff_t* pos)
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
static int procfs_open_full(struct inode* inode, struct file* file)
{
    return seq_open(file, &seq_ops);
}

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open_single(struct inode* inode, struct file* file)
{
    return single_open(file, single_show, NULL);
}

static void timer_handler(struct timer_list* timer)
{
    data_t* data;
    printk(KERN_INFO "Timer handler called\n");
    // Reset the timer
    timer->expires = jiffies + (TIMER_DELAY * HZ);
    add_timer(timer);
    // Add a new data node
    data = create_data_node(&data_list, &list_len, GFP_ATOMIC);
    populate_data(data);
    return;
}

static int __init hello(void)
{
    printk(KERN_INFO "Hello world\n");
    // proc_dir = proc_mkdir(procfs_dir_name, NULL);
    // proc_file_single = proc_create(procfs_name_single, 0444, proc_dir, &f_ops_single);
    // proc_file_full = proc_create(procfs_name_full, 0444, proc_dir, &f_ops_full);
    // // Create and start timer
    // timer_setup(&data_timer, timer_handler, 0);
    // data_timer.expires = jiffies + (TIMER_DELAY * HZ);
    // add_timer(&data_timer);
    // list_len = 0;
    sysfs_entry = kobject_create_and_add("sentinel", kernel_kobj);
    sysfs_entry->ktype->sysfs_ops = &sentinel_sysops;
    if (!sysfs_entry) {
        return -ENOMEM;
    }
    attr.name = "sentinel_entry";
    attr.mode = S_IRUGO;
    sysfs_create_file(sysfs_entry, &attr);
    return 0;
}

static void __exit goodbye(void)
{
    free_list(&data_list);
    // proc_remove(proc_file_single);
    // proc_remove(proc_file_full);
    // proc_remove(proc_dir);
    // del_timer(&data_timer);
    sysfs_remove_file(sysfs_entry, &attr);
    kobject_del(sysfs_entry);
    printk(KERN_INFO "Goodbye world\n");
}

module_init(hello);
module_exit(goodbye);