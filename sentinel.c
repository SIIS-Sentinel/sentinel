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
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#define procfs_name "sentinel"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("0.1");

static int __initdata data = 3;
struct proc_dir_entry* proc_file;

int procfile_read(struct seq_file* m, void* v)
{
    seq_write(m, "Welcome to sentinel\n", 21);
    return 0;
}

int output_memory_use(void)
{
    struct sysinfo i;
    long available;
    void (*si_swapinfo)(struct sysinfo*);
    // Get the address of si_swapinfo
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    // Populate the systinfo struct
    si_meminfo(&i);
    si_swapinfo(&i);
    available = si_mem_available();
    // Output stats
    printk(KERN_INFO "Total Memory: %lu kB", i.totalram << (PAGE_SHIFT - 10));
    return 0;
}

static int __init hello(void)
{
    printk(KERN_INFO "Hello world %d\n", data);
    // Create the proc file
    proc_file = proc_create_single(procfs_name, 0, NULL, procfile_read);
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