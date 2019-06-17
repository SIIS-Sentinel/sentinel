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
#define procfs_name_iter "iter"

static int single_show(struct seq_file* seq, void* v);
static int procfs_open(struct inode* inode, struct file* file);
static int __init hello(void);
static void __exit goodbye(void);

static struct file_operations f_ops = {
    .owner = THIS_MODULE,
    .open = procfs_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release
};

struct proc_dir_entry* proc_dir;
struct proc_dir_entry* proc_file_single;
struct proc_dir_entry* proc_file_iter;
LIST_HEAD(data_list);

#endif // SENTINEL_H