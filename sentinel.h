#ifndef SENTINEL_H
#define SENTINEL_H

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
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>

#include "sentinel_procfs.h"

#define procfs_dir_name "sentinel"
#define procfs_name_single "single"
#define procfs_name_full "full"

static int __init hello(void);
static void __exit goodbye(void);

#endif // SENTINEL_H