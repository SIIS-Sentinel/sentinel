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
#include <linux/sched/loadavg.h>
#include <linux/slab.h>

#include "sentinel_helper.h"
#include "sentinel_sysfs.h"

#define NB_FILES_NOT_CPU 17

int nb_files;
struct kobject* sysfs_entry;
struct kobj_attribute* k_attrs;
struct attribute** attrs;
struct attribute_group attr_group;

ssize_t sentinel_sysfs_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    int i;
    char name[32];
    data_t data;
    populate_data(&data);
    printk(KERN_INFO "Sentinel sysfs show called\n");
    // Check if we called a CPU file
    for (i = 0; i < data.nb_cpus; i++) {
        memset(name, 0, sizeof(name));
        sprintf(name, "CPU_%d", i);
        if (strcmp(attr->attr.name, name) == 0) {
            printk(KERN_INFO "Returning freq of CPU %d\n", i);
            return sprintf(buf, "%u\n", data.cpu_freq[i]);
        }
    }
    // Check if we called another file
    if (strcmp(attr->attr.name, "nb_cpus") == 0) {
        return sprintf(buf, "%u\n", data.nb_cpus);
    } else if (strcmp(attr->attr.name, "total_ram") == 0) {
        return sprintf(buf, "%ld\n", data.totalram << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "free_ram") == 0) {
        return sprintf(buf, "%ld\n", data.freeram << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "used_ram") == 0) {
        return sprintf(buf, "%ld\n", data.usedram << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "total_swap") == 0) {
        return sprintf(buf, "%ld\n", data.totalswap << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "free_swap") == 0) {
        return sprintf(buf, "%ld\n", data.freeswap << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "used_swap") == 0) {
        return sprintf(buf, "%ld\n", data.usedswap << (PAGE_SHIFT - 10));
    } else if (strcmp(attr->attr.name, "nb_processes") == 0) {
        return sprintf(buf, "%u\n", data.nb_processes);
    } else if (strcmp(attr->attr.name, "load_1m") == 0) {
        return sprintf(buf, "%lu.%02lu\n", LOAD_INT(data.loads[0]), LOAD_FRAC(data.loads[0]));
    } else if (strcmp(attr->attr.name, "load_5m") == 0) {
        return sprintf(buf, "%lu.%02lu\n", LOAD_INT(data.loads[1]), LOAD_FRAC(data.loads[1]));
    } else if (strcmp(attr->attr.name, "load_15m") == 0) {
        return sprintf(buf, "%lu.%02lu\n", LOAD_INT(data.loads[2]), LOAD_FRAC(data.loads[2]));
    } else if (strcmp(attr->attr.name, "tracked_pid") == 0) {
        return sprintf(buf, "%u\n", data.tracked_pid);
    } else if (strcmp(attr->attr.name, "task_size") == 0) {
        return sprintf(buf, "%lu\n", data.mm_size);
    } else if (strcmp(attr->attr.name, "hiwater_rss") == 0) {
        return sprintf(buf, "%lu\n", data.mm_hiwater_rss);
    } else if (strcmp(attr->attr.name, "hiwater_vm") == 0) {
        return sprintf(buf, "%lu\n", data.mm_hiwater_vm);
    } else if (strcmp(attr->attr.name, "total_vm") == 0) {
        return sprintf(buf, "%lu\n", data.mm_total_vm);
    } else if (strcmp(attr->attr.name, "nb_fds") == 0) {
        return sprintf(buf, "%u\n", data.nb_files);
    }
    return -EIO;
}

ssize_t sentinel_sysfs_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t len)
{
    printk(KERN_INFO "Sentinel sysfs store called\n");
    if (strcmp(attr->attr.name, "tracked_pid") == 0) {
        // We are changing the tracked PID
        change_tracked_pid(buf, len);
        return len;
    } else {
        return -EIO;
    }
}

uint32_t init_attr_group(void)
{
    int i;
    char buf[32];
    char* name;
    data_t data;
    // Calculate the number of files needed
    populate_data(&data);
    nb_files = data.nb_cpus + NB_FILES_NOT_CPU;
    // Allocate as many struct kobj_attribute as needed, and fill them out with __ATTR
    k_attrs = kcalloc(nb_files, sizeof(struct kobj_attribute), GFP_KERNEL);
    // Populate k_attrs
    for (i = 0; i < nb_files; i++) {
        memset(buf, 0, sizeof(buf));
        if (i < data.nb_cpus) {
            sprintf(buf, "CPU_%d", i);
        } else {
            printk(KERN_INFO "switch on %d\n", i - data.nb_cpus);
            switch (i - data.nb_cpus) {
            case 0:
                sprintf(buf, "nb_cpus");
                break;
            case 1:
                sprintf(buf, "total_ram");
                break;
            case 2:
                sprintf(buf, "free_ram");
                break;
            case 3:
                sprintf(buf, "used_ram");
                break;
            case 4:
                sprintf(buf, "total_swap");
                break;
            case 5:
                sprintf(buf, "free_swap");
                break;
            case 6:
                sprintf(buf, "used_swap");
                break;
            case 7:
                sprintf(buf, "nb_processes");
                break;
            case 8:
                sprintf(buf, "load_1m");
                break;
            case 9:
                sprintf(buf, "load_5m");
                break;
            case 10:
                sprintf(buf, "load_15m");
                break;
            case 11:
                sprintf(buf, "tracked_pid");
                break;
            case 12:
                sprintf(buf, "task_size");
                break;
            case 13:
                sprintf(buf, "hiwater_rss");
                break;
            case 14:
                sprintf(buf, "hiwater_vm");
                break;
            case 15:
                sprintf(buf, "total_vm");
                break;
            case 16:
                sprintf(buf, "nb_fds");
                break;
            default:
                printk(KERN_ERR "WTF\n");
                break;
            }
        }
        name = kstrdup(buf, GFP_KERNEL);
        k_attrs[i].attr.name = name;
        k_attrs[i].attr.mode = 0664;
        k_attrs[i].show = sentinel_sysfs_show;
        k_attrs[i].store = sentinel_sysfs_store;
    }
    // Allocate a struct attribute* array of the correct size
    attrs = kcalloc(nb_files + 1, sizeof(struct attribute*), GFP_KERNEL);
    // Store the address of the attr member of each struct kobj_attribute in the array
    for (i = 0; i < nb_files; i++) {
        attrs[i] = &k_attrs[i].attr;
    }
    attr_group.attrs = attrs;
    return 0;
}

uint32_t del_attr_group(void)
{
    int i;
    for (i = 0; i < nb_files; i++) {
        kfree(k_attrs[i].attr.name);
    }
    kfree(k_attrs);
    kfree(attrs);
    return 0;
}