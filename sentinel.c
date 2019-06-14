#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/cpu.h>
#include <linux/cpufreq.h>
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

#define procfs_dir_name "sentinel"
#define procfs_name_single "single"
#define procfs_name_iter "iter"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("1.0");

struct proc_dir_entry* proc_dir;
struct proc_dir_entry* proc_file_single;
struct proc_dir_entry* proc_file_iter;

static void show_val_kb(struct seq_file* seq, const char* s, unsigned long num)
{
    num = num << (PAGE_SHIFT - 10);
    seq_printf(seq, "%s:\t%*ld kB\n", s, 8, num);
}

static void show_cpu_freq(struct seq_file* seq, loff_t cpu_id, struct cpuinfo_x86 cinfo)
{
    unsigned int freq;
    if (cpu_has(&cinfo, X86_FEATURE_TSC)) {
        freq = cpufreq_quick_get(cpu_id);
        if (!freq)
            freq = cpu_khz;
        seq_printf(seq, "CPU frequency:\t %u.%03u MHz\n", freq / 1000, (freq % 1000));
    }
}

static int single_show(struct seq_file* seq, void* v)
{
    struct sysinfo info;
    struct cpuinfo_x86 cinfo;
    loff_t cpu_id;
    void (*si_swapinfo)(struct sysinfo*);
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Buffer size: %ld\n", seq->size);

    // Header
    seq_printf(seq, "Welcome to Sentinel\n");

    // Memory
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    si_meminfo(&info);
    si_swapinfo(&info);
    show_val_kb(seq, "Total memory", info.totalram);
    show_val_kb(seq, "Free memory", info.freeram);
    show_val_kb(seq, "Used memory", info.totalram - info.freeram);
    show_val_kb(seq, "Total swap", info.totalswap);
    show_val_kb(seq, "Free swap", info.freeswap);
    show_val_kb(seq, "Used swap", info.totalswap - info.freeswap);

    // CPU
    cpu_id = 0;
    while (cpu_id < nr_cpu_ids) {
        // Get CPU data from ID
        cinfo = cpu_data(cpu_id);
        show_cpu_freq(seq, cpu_id, cinfo);
        // Get next valid ID
        cpu_id = cpumask_next(cpu_id, cpu_online_mask);
    }
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
    proc_dir = proc_mkdir(procfs_dir_name, NULL);
    proc_file_single = proc_create(procfs_name_single, 0444, proc_dir, &f_ops);
    proc_file_iter = proc_create(procfs_name_iter, 0444, proc_dir, &f_ops);
    return 0;
}

static void __exit goodbye(void)
{
    proc_remove(proc_file_single);
    proc_remove(proc_file_iter);
    proc_remove(proc_dir);
    printk(KERN_INFO "Goodbye world\n");
}

// fs_initcall(hello);
module_init(hello);
module_exit(goodbye);