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
#include <linux/module.h>
#include <linux/seq_file.h>

#include "sentinel_sysfs.h"

struct kobject* sysfs_entry;
struct attribute attr;

const struct sysfs_ops sentinel_sysops = {
    .show = sentinel_sysfs_show,
    .store = sentinel_sysfs_store,
};

ssize_t sentinel_sysfs_show(struct kobject* kobj, struct attribute* attr, char* buf)
{
    printk(KERN_INFO "sysfs show called\n");
    return sprintf(buf, "1\n");
}

ssize_t sentinel_sysfs_store(struct kobject* kobj, struct attribute* attr, const char* buf, size_t len)
{
    printk(KERN_INFO "sysfs show called\n");
    return -EIO;
}
