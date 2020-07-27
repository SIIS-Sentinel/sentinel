#ifndef SENTINEL_HELPER_H
#define SENTINEL_HELPER_H

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

#define MAX_NR_CPU 32
#define MAX_LIST_LEN 32
#define TIMER_DELAY 5

typedef struct hardware_data {
    long nsecs;
    time64_t secs;
    unsigned long totalram;
    unsigned long freeram;
    unsigned long usedram;
    unsigned long totalswap;
    unsigned long freeswap;
    unsigned long usedswap;
    unsigned int nb_cpus;
    unsigned int cpu_freq[MAX_NR_CPU];
    struct list_head list;
    unsigned int nb_processes;
    unsigned long loads[3];
    uint32_t tracked_pid;
    unsigned long mm_size;
    unsigned long mm_hiwater_rss;
    unsigned long mm_hiwater_vm;
    unsigned long mm_total_vm;
    unsigned int nb_files;
} data_t;

void populate_data(data_t* data);
void change_tracked_pid(const char* buf, size_t len);
void get_process_data(uint32_t pid, data_t* data);
#endif // SENTINEL_HERLER_H
