#ifndef SENTINEL_PROCFS_H
#define SENTINEL_PROCFS_H

#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#define __NO_VERSION__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h>

void* procfile_start(struct seq_file* seq, loff_t* pos);
void* procfile_next(struct seq_file* seq, void* v, loff_t* pos);
int procfile_show(struct seq_file* seq, void* v);
void procfile_stop(struct seq_file* seq, void* v);
int single_show(struct seq_file* seq, void* v);
int procfs_open_full(struct inode* inode, struct file* file);
int procfs_open_single(struct inode* inode, struct file* file);
void timer_handler(struct timer_list* timer);

// Seq file ops
extern struct file_operations f_ops_full;
extern struct file_operations f_ops_single;

// Misc variables
extern struct proc_dir_entry* proc_dir;
extern struct proc_dir_entry* proc_file_single;
extern struct proc_dir_entry* proc_file_full;
extern struct list_head data_list;
extern struct timer_list data_timer;
extern int list_len;

#endif // SENTINEL_PROCFS_H