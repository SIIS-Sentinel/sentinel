#ifndef SENTINEL_H
#define SENTINEL_H

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
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>

#define procfs_dir_name "sentinel"
#define procfs_name_single "single"
#define procfs_name_full "full"
#define TIMER_DELAY 5

static void* procfile_start(struct seq_file* seq, loff_t* pos);
static void* procfile_next(struct seq_file* seq, void* v, loff_t* pos);
static int procfile_show(struct seq_file* seq, void* v);
static void procfile_stop(struct seq_file* seq, void* v);
static int single_show(struct seq_file* seq, void* v);
static int procfs_open_full(struct inode* inode, struct file* file);
static int procfs_open_single(struct inode* inode, struct file* file);
static void timer_handler(struct timer_list* timer);
static int __init hello(void);
static void __exit goodbye(void);

// Seq file ops
static struct seq_operations seq_ops = {
    .start = procfile_start,
    .next = procfile_next,
    .stop = procfile_stop,
    .show = procfile_show
};

static struct file_operations f_ops_full = {
    .owner = THIS_MODULE,
    .open = procfs_open_full,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release
};

static struct file_operations f_ops_single = {
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

#endif // SENTINEL_H