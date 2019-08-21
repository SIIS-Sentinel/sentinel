#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#include "sentinel_helper.h"

#define __NO_VERSION__

data_t* create_data_node(struct list_head* list, int* list_len, gfp_t flag)
{
    data_t* data;
    data = kmalloc(sizeof(data_t), flag);
    if (!data) {
        return NULL;
    }
    INIT_LIST_HEAD(&data->list);
    list_add_tail(&data->list, list);
    // Keep the list at a fixed max size
    (*list_len)++;
    if (*list_len >= MAX_LIST_LEN) {
        data = list_entry(list->next, data_t, list);
        list_del(&data->list);
        kfree(data);
        (*list_len)--;
    }
    return data;
}

void show_time(struct seq_file* seq, time_t secs)
{
    seq_printf(seq, "Measurement time:\t%ld s\n", secs);
}

void show_val_kb(struct seq_file* seq, const char* s, unsigned long num)
{
    num = num << (PAGE_SHIFT - 10);
    seq_printf(seq, "%s:\t%*ld kB\n", s, 8, num);
}

void show_cpu_freq(struct seq_file* seq, loff_t cpu_id, unsigned int freq)
{
    seq_printf(seq, "CPU %lld frequency:\t %u.%03u MHz\n", cpu_id, freq / 1000, (freq % 1000));
}

void populate_data(data_t* data)
{
    struct sysinfo info;
    loff_t cpu_id;
    unsigned int freq;
    void (*si_swapinfo)(struct sysinfo*);
    struct timespec64 tv;

    // Time
    ktime_get_real_ts64(&tv);
    data->secs = tv.tv_sec;
    data->nsecs = tv.tv_nsec;
    // Memory data
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    si_meminfo(&info);
    si_swapinfo(&info);
    data->totalram = info.totalram;
    data->freeram = info.freeram;
    data->usedram = info.totalram - info.freeram;
    data->totalswap = info.totalswap;
    data->freeswap = info.freeswap;
    data->usedswap = info.totalswap - info.freeswap;

    // Get CPU data
    data->nb_cpus = 0;
    cpu_id = 0;
    while (cpu_id < nr_cpu_ids) {
        // Get CPU freq from ID
        freq = cpufreq_quick_get(cpu_id);
        if (!freq) {
            freq = cpu_khz;
        }
        data->cpu_freq[data->nb_cpus] = freq;
        // Get next valid ID
        cpu_id = cpumask_next(cpu_id, cpu_online_mask);
        data->nb_cpus++;
    }
    return;
}

void print_data_verbose(struct seq_file* seq, const data_t data)
{
    loff_t cpu_id;
    // Time
    show_time(seq, data.secs);
    // Memory
    show_val_kb(seq, "Total memory", data.totalram);
    show_val_kb(seq, "Free memory", data.freeram);
    show_val_kb(seq, "Used memory", data.usedram);
    show_val_kb(seq, "Total swap", data.totalswap);
    show_val_kb(seq, "Free swap", data.freeswap);
    show_val_kb(seq, "Used swap", data.usedswap);

    // CPU
    cpu_id = 0;
    while (cpu_id < data.nb_cpus) {
        show_cpu_freq(seq, cpu_id, data.cpu_freq[cpu_id]);
        cpu_id++;
    }
    return;
}

void print_data_short(struct seq_file* seq, struct list_head* list, void* v)
{
    data_t* tmp;
    if (v == list) {
        seq_printf(seq, "Time (s), Total RAM, Free RAM, Used RAM, Total Swap, Free Swap, Used Swap, #CPUs\n");
    } else {
        tmp = list_entry(v, data_t, list);
        seq_printf(seq,
            "%lld,  %lu, %lu, %lu, %lu, %lu, %lu, %u\n",
            tmp->secs,
            tmp->totalram, tmp->freeram, tmp->usedram,
            tmp->totalswap, tmp->freeswap, tmp->usedswap,
            tmp->nb_cpus);
    }
}

void free_list(struct list_head* list_head)
{
    data_t *pos, *next;
    list_for_each_entry_safe(pos, next, list_head, list)
    {
        list_del(&pos->list);
        kfree(pos);
    }
    return;
}