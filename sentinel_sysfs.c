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
#include <linux/slab.h>

#include "sentinel_helper.h"
#include "sentinel_sysfs.h"

int num_files;
struct kobject* sysfs_entry;
struct kobj_attribute* k_attrs;
struct attribute** attrs;
struct attribute_group attr_group;

// const struct sysfs_ops sentinel_sysops = {
//     .show = sentinel_sysfs_show,
//     .store = sentinel_sysfs_store,
// };

ssize_t sentinel_sysfs_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    data_t data;
    populate_data(&data);
    printk(KERN_INFO "sysfs show called\n");
    return sprintf(buf, "%ld\n", data.freeram << (PAGE_SHIFT - 10));
}

ssize_t sentinel_sysfs_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t len)
{
    printk(KERN_INFO "sysfs show called\n");
    return -EIO;
}

uint32_t init_attr_group(void)
{
    int i;
    char* name;
    // Calculate the number of files needed
    printk("Here\n");
    num_files = 1;
    // Allocate as many struct kobj_attribute as needed, and fill them out with __ATTR
    k_attrs = kcalloc(num_files, sizeof(struct kobj_attribute), GFP_KERNEL);
    // Populate k_attrs
    for (i = 0; i < num_files; i++) {
        name = kstrdup("file", GFP_KERNEL);
        k_attrs[i].attr.name = name;
        k_attrs[i].attr.mode = 0664;
        k_attrs[i].show = sentinel_sysfs_show;
        k_attrs[i].store = sentinel_sysfs_store;
    }
    printk("There\n");
    // Allocate a struct attribute* array of the correct size
    attrs = kcalloc(num_files + 1, sizeof(struct attribute*), GFP_KERNEL);
    // Store the address of the attr member of each struct kobj_attribute in the array
    printk("Over here\n");
    for (i = 0; i < num_files; i++) {
        attrs[i] = &k_attrs[i].attr;
    }
    attr_group.attrs = attrs;
    printk("What about here\n");
    return 0;
}

uint32_t del_attr_group(void)
{
    int i;
    for (i = 0; i < num_files; i++) {
        kfree(k_attrs[i].attr.name);
    }
    kfree(k_attrs);
    kfree(attrs);
    return 0;
}