#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#define PROCFS

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/timer.h>

#include "sentinel.h"
#include "sentinel_helper.h"
#include "sentinel_procfs.h"

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

static int __init hello(void)
{
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
    sysfs_entry = kobject_create_and_add("sentinel", kernel_kobj);
    sysfs_entry->ktype->sysfs_ops = &sentinel_sysops;
    if (!sysfs_entry) {
        return -ENOMEM;
    }
    attr.name = "sentinel_entry";
    attr.mode = S_IRUGO;
    sysfs_create_file(sysfs_entry, &attr);
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
    sysfs_remove_file(sysfs_entry, &attr);
    kobject_del(sysfs_entry);
    printk(KERN_INFO "Sentinel terminated\n");
}

module_init(hello);
module_exit(goodbye);