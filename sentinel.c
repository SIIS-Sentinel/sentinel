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
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "sentinel.h"
#include "sentinel_helper.h"
#include "sentinel_procfs.h"
#include "sentinel_sysfs.h"

// #define PROCFS
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
#ifdef PROCFS
    proc_dir = proc_mkdir(procfs_dir_name, NULL);
    proc_file_single = proc_create(procfs_name_single, 0444, proc_dir, &f_ops_single);
    proc_file_full = proc_create(procfs_name_full, 0444, proc_dir, &f_ops_full);
    // Create and start timer
    timer_setup(&data_timer, timer_handler, 0);
    data_timer.expires = jiffies + (TIMER_DELAY * HZ);
    add_timer(&data_timer);
    list_len = 0;
#endif
#ifdef SYSFS
    sysfs_entry = kobject_create_and_add("sentinel", kernel_kobj);
    init_attr_group();
    retval = sysfs_create_group(sysfs_entry, &attr_group);
    // sysfs_entry->ktype->sysfs_ops = &sentinel_sysops;
    // if (!sysfs_entry) {
    //     return -ENOMEM;
    // }
    // attr.name = "sentinel_entry";
    // attr.mode = S_IRUGO;
    // sysfs_create_file(sysfs_entry, &attr);
    // Create a struct attribute_group, and store the array in its attrs member
    // If the return code is not 0, call kobject_put to abort it all, and deallocate everything
#endif
    printk(KERN_INFO "Sentinel initialized\n");
    return 0;
}

static void __exit goodbye(void)
{
#ifdef PROCFS
    free_list(&data_list);
    proc_remove(proc_file_single);
    proc_remove(proc_file_full);
    proc_remove(proc_dir);
    del_timer(&data_timer);
#endif
#ifdef SYSFS
    // sysfs_remove_file(sysfs_entry, &attr);
    kobject_put(sysfs_entry);
    del_attr_group();
    // Call kobject_put to cleanup stuff easily
#endif
    printk(KERN_INFO "Sentinel terminated\n");
}

module_init(hello);
module_exit(goodbye);