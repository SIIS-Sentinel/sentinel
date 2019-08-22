#ifndef SENTINEL_SYSFS_H
#define SENTINEL_SYSFS_H

#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#define __NO_VERSION__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

extern struct kobject* sysfs_entry;
extern struct attribute attr;
extern const struct sysfs_ops sentinel_sysops;

ssize_t sentinel_sysfs_show(struct kobject* kobj, struct attribute* attr, char* buf);
ssize_t sentinel_sysfs_store(struct kobject* kobj, struct attribute* attr, const char* buf, size_t len);

#endif // SENTINEL_SYSFS_H