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
#include <linux/module.h>

static int __init hello(void);
static void __exit goodbye(void);

#endif // SENTINEL_H
