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
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#include "sentinel_helper.h"

#define __NO_VERSION__

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

    // Time
    data->time = ktime_get_real_seconds();
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

void print_data(struct seq_file* seq, const data_t data)
{
    loff_t cpu_id;
    // Time
    show_time(seq, data.time);
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
