#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

// #include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "sentinel.h"
#include "sentinel_helper.h"
#include "sentinel_sysfs.h"

#define SYSFS

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("2.0");

uint32_t tracked_pid;

static int __init hello(void)
{
    int retval;
    tracked_pid = 0;
#ifdef SYSFS
    // Create the sentinel folder in /sys/kernel
    sysfs_entry = kobject_create_and_add("sentinel", kernel_kobj);
    // Generate all the files in the sentinel folder
    init_attr_group();
    retval = sysfs_create_group(sysfs_entry, &attr_group);
#endif
    printk(KERN_INFO "Sentinel initialized\n");
    return 0;
}

static void __exit goodbye(void)
{
#ifdef SYSFS
    // Call kobject_put to cleanup stuff easily
    kobject_put(sysfs_entry);
    del_attr_group();
#endif
    printk(KERN_INFO "Sentinel terminated\n");
}

module_init(hello);
module_exit(goodbye);
