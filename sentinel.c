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
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#define procfs_name "sentinel"
#define MESSAGE "Welcome to Sentinel\n"
#define BUF_SIZE 64

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("1.0");

struct proc_dir_entry* proc_file;

static void show_val_kb(struct seq_file* seq, const char* s, unsigned long num)
{
    num = num << (PAGE_SHIFT - 10);
    seq_printf(seq, "%s:\t%*ld kB\n", s, 8, num);
}

static int single_show(struct seq_file* seq, void* v)
{
    struct sysinfo info;
    void (*si_swapinfo)(struct sysinfo*);
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Buffer size: %ld\n", seq->size);

    // Gather the data
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    si_meminfo(&info);
    si_swapinfo(&info);

    seq_printf(seq, "Welcome to Sentinel\n");
    // Copy the total memory
    show_val_kb(seq, "Total memory", info.totalram);
    // Copy the free memory
    show_val_kb(seq, "Free memory", info.freeram);
    // Copy the total memory
    show_val_kb(seq, "Used memory", info.totalram - info.freeram);
    return 0;
}

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open(struct inode* inode, struct file* file)
{
    return single_open(file, single_show, NULL);
    // return seq_open(file, &seq_ops);
}

// File ops struct
static struct file_operations f_ops = {
    .owner = THIS_MODULE,
    .open = procfs_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release
};

static int __init hello(void)
{
    printk(KERN_INFO "Hello world\n");
    proc_file = proc_create(procfs_name, 0444, NULL, &f_ops);
    return 0;
}

static void __exit goodbye(void)
{
    proc_remove(proc_file);
    printk(KERN_INFO "Goodbye world\n");
}

// fs_initcall(hello);
module_init(hello);
module_exit(goodbye);