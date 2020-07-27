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
#include <linux/fdtable.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#include "sentinel_helper.h"

#define __NO_VERSION__

extern uint32_t tracked_pid;

// Fills the given data structure by gathering all the metrics from the kernel
void populate_data(data_t* data)
{
    struct sysinfo info;
    loff_t cpu_id;
    unsigned int freq;
    void (*si_swapinfo)(struct sysinfo*);
    void (*get_avenrun)(unsigned long*, unsigned long, int);
    struct timespec64 tv;
    struct task_struct* task;

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

    // CPU number and frequencies
    data->nb_cpus = 0;
    cpu_id = 0;
    while (cpu_id < nr_cpu_ids) {
        // Get CPU freq from ID
        freq = cpufreq_quick_get(cpu_id);
        // The lines below are needed on some systems, but make compilation fail on other
        // TODO: determine how to handle that automaticallys
        // if (!freq) {
        //     freq = cpu_khz;
        // }
        data->cpu_freq[data->nb_cpus] = freq;
        // Get next valid ID
        cpu_id = cpumask_next(cpu_id, cpu_online_mask);
        data->nb_cpus++;
    }
    // CPU average loads
    get_avenrun = (void (*)(unsigned long*, unsigned long, int))kallsyms_lookup_name("get_avenrun");
    get_avenrun(data->loads, FIXED_1 / 200, 0);

    // Number of processes
    data->nb_processes = 0;
    for_each_process(task)
    {
        data->nb_processes++;
    }

    // Tracked PID value
    data->tracked_pid = tracked_pid;

    // Tracked process data
    get_process_data(data->tracked_pid, data);
    return;
}

// Converts the given PID from char[] to int, and stores it
void change_tracked_pid(const char* buf, size_t len)
{
    uint32_t tmp_pid;
    if (kstrtou32(buf, 10, &tmp_pid) == 0) {
        tracked_pid = tmp_pid;
    }
}

// Fills out the relevant fields in data with the
// process information of the given PID
void get_process_data(uint32_t pid, data_t* data)
{
    int files_counter;
    struct file* tmp_file;
    struct task_struct* task;
    struct mm_struct* mm;
    struct files_struct* files;
    struct fdtable* fdt;

    // Fetch the relevant process
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (task == NULL) {
        printk(KERN_INFO "Process fetch failed\n");
        return;
    }
    mm = task->mm;
    if (mm == NULL) {
        mm = task->active_mm;
    }
    if (mm == NULL) {
        // Panic mode
        printk(KERN_INFO "MM fetch failed\n");
        return;
    }
    files = task->files;

    // Memory information
    data->mm_size = mm->task_size;
    data->mm_hiwater_rss = mm->hiwater_rss;
    data->mm_hiwater_vm = mm->hiwater_vm;
    data->mm_total_vm = mm->total_vm;

    // File handles information
    rcu_read_lock();
    fdt = files_fdtable(files);
    files_counter = 0;
    tmp_file = fdt->fd[0];
    while (tmp_file != NULL) {
        files_counter++;
        tmp_file = fdt->fd[files_counter];
    }
    data->nb_files = files_counter;
    rcu_read_unlock();
}
