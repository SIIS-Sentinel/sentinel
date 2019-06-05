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
#include <linux/swap.h>
#include <linux/sysinfo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("0.1");

static int __initdata data = 3;

static int __init hello(void)
{
    struct sysinfo i;
    long available;
    void (*si_swapinfo)(struct sysinfo*);
    printk(KERN_INFO "Hello world %d\n", data);
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

static void __exit goodbye(void)
{
    printk(KERN_INFO "Goodbye world\n");
}

module_init(hello);
module_exit(goodbye);