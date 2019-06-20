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
#include <linux/seq_file.h>
#include <linux/time.h>

#define MAX_NR_CPU 32

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
} data_t;

data_t* create_data_node(struct list_head* list, gfp_t flag);
void show_time(struct seq_file* seq, time_t secs);
void show_val_kb(struct seq_file* seq, const char* s, unsigned long num);
void show_cpu_freq(struct seq_file* seq, loff_t cpu_id, unsigned int freq);
void populate_data(data_t* data);
void print_data_verbose(struct seq_file* seq, const data_t data);
void print_data_short(struct seq_file* seq, struct list_head* list, void* v);
void free_list(struct list_head* list_head);

#endif // SENTINEL_HERLER_H
