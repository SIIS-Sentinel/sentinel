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
// extern struct attribute attr;
// extern const struct sysfs_ops sentinel_sysops;
extern struct attribute_group attr_group;

ssize_t sentinel_sysfs_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf);
ssize_t sentinel_sysfs_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t len);
uint32_t init_attr_group(void);
uint32_t del_attr_group(void);

#endif // SENTINEL_SYSFS_H