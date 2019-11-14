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

data_t* create_data_node(struct list_head* list, int* list_len, gfp_t flag);
void show_time(struct seq_file* seq, time_t secs);
void show_val_kb(struct seq_file* seq, const char* s, unsigned long num);
void show_cpu_freq(struct seq_file* seq, loff_t cpu_id, unsigned int freq);
void show_loads(struct seq_file* seq, const unsigned long* loads);
void show_processes(struct seq_file* seq, unsigned int nb_processes);
void populate_data(data_t* data);
void print_data_verbose(struct seq_file* seq, const data_t data);
void print_data_short(struct seq_file* seq, struct list_head* list, void* v);
void free_list(struct list_head* list_head);
void change_tracked_pid(const char* buf, size_t len);
void get_process_data(uint32_t pid, data_t* data);
#endif // SENTINEL_HERLER_H
