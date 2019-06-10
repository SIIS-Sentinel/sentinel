#ifndef LINUX
#define LINUX
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

#define procfs_name "sentinel"
#define MESSAGE "Welcome to Sentinel\n"
#define BUF_SIZE 64

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("1.0");

static int __initdata data = 3;
struct proc_dir_entry* proc_file;
static char message[BUF_SIZE];
static char* msg_ptr;
static long int msg_len;
static long int counter;
static long int max_iter = 1000;

// Called by procfile_read() to copy some data into the user buffer
// Args:
//  user_buf: destination buffer
//  text: message to prepend to the data
//  offset: offset to use when writing in the buffer
//  data: data to write in the buffer
// Return value:
//  new offset
int copy_data(char* __user user_buf, const char* text, int offset, unsigned long data)
{
    char data_buf[BUF_SIZE];
    sprintf(data_buf, "%s%lu kB\n", text, data << (PAGE_SHIFT - 10));
    copy_to_user(user_buf + offset, data_buf, strlen(data_buf));
    offset += strlen(data_buf);
    return offset;
}

// Called when /proc/sentinel is read
// Args:
//  file: not sure yet (file desc of the /proc/sentinel file?)
//  buffer: array where the data should be copied to
//  len: length of the buffer
//  offset: offset where the read should start at (here used as a counter)
// Return value:
//  number of bytes copied in the the buffer. The function keeps being called until the number is 0
ssize_t procfile_read(struct file* file, char __user* buffer, size_t len, loff_t* offset)
{
    int bytes_copied = 0;
    struct sysinfo info;
    void (*si_swapinfo)(struct sysinfo*);
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Length: %ld, offset: %lld\n", len, *offset);

    // Gather the data
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    si_meminfo(&info);
    si_swapinfo(&info);

    switch (*offset) {
    case 0:
        // Copy the message
        copy_to_user(buffer + bytes_copied, message, msg_len);
        bytes_copied += msg_len;
        break;
    case 1:
        // Copy the total memory
        bytes_copied = copy_data(buffer, "Total memory:\t", bytes_copied, info.totalram);
        break;
    case 2:
        // Copy the free memory
        bytes_copied = copy_data(buffer, "Free memory:\t", bytes_copied, info.freeram);
        break;
    case 3:
        // Copy the total memory
        bytes_copied = copy_data(buffer, "Used memory:\t", bytes_copied, info.totalram - info.freeram);
        break;
    default:
        // We are done with reading
        return 0;
        break;
    }
    (*offset)++;

    printk(KERN_INFO "Number of bytes copied: %d", bytes_copied);
    return bytes_copied;
}

// Called when /proc/sentinel is written to
// Args:
//  file: not sure yet (file desc of the /proc/sentinel file?)
//  buffer: array where the data is located
//  len: length of the contents (in bytes, including a \0 if present)
//  offset: offset where the last write stopped
// Return value:
//  number of bytes copied from the buffer. The function keeps being called until out == len
// Notes:
//  when the function is called multiple times on the same data, the buffer is changed to remove the written data
ssize_t procfile_write(struct file* file, const char __user* buffer, size_t len, loff_t* offset)
{
    char kbuf[BUF_SIZE];
    printk(KERN_INFO "Write handler of Sentinel called\n");
    printk(KERN_INFO "Length: %ld, offset: %lld\n", len, *offset);
    // Make sure we do not overflow the buffer
    if (len > BUF_SIZE) {
        printk(KERN_INFO "Truncating to buffer size\n");
        len = BUF_SIZE;
    }
    copy_from_user(kbuf, buffer, len);
    printk(KERN_INFO "Message: %.*s", (int)len, kbuf);
    (*offset)++;
    return len;
}

int output_memory_use(void)
{
    struct sysinfo i;
    long available;
    void (*si_swapinfo)(struct sysinfo*);
    // Get the address of si_swapinfo
    si_swapinfo = (void (*)(struct sysinfo*))kallsyms_lookup_name("si_swapinfo");
    // Populate the systinfo struct
    si_meminfo(&i);
    si_swapinfo(&i);
    available = si_mem_available();
    // Output stats
    printk(KERN_INFO "Total Memory: %lu kB", i.totalram << (PAGE_SHIFT - 10));
    return 0;
}

static void* procfile_start(struct seq_file* seq, loff_t* pos)
{
    printk(KERN_INFO "Entering procfile_start with pos==%lld\n", *pos);
    if (*pos == 0) {
        // If we are at the start of the file, return &counter
        counter = 0;
        return &counter;
    } else if (*pos < max_iter) {
        // We are in the file, keep going
        return &counter;
    } else {
        // We are at the end of the file, stop
        *pos = 0;
        return NULL;
    }
}
static void* procfile_next(struct seq_file* seq, void* v, loff_t* pos)
{
    // Print the entire data
    long int* iterator = (long int*)v;
    printk(KERN_INFO "Entering procfile_next\n");
    ++(*iterator);
    ++(*pos);
    if (*pos == max_iter) {
        printk(KERN_INFO "Stopping due to *pos==%ld\n", max_iter);
        return NULL;
    } else {
        printk(KERN_INFO "Proceeding\n");
        return iterator;
    }
}

static void procfile_stop(struct seq_file* seq, void* v)
{
    // Nothing to do here
    printk(KERN_INFO "Entering procfile_stop\n");
    return;
}

static int procfile_show(struct seq_file* seq, void* v)
{
    // Print the iterator
    long int* tmp_v = (long int*)v;
    printk(KERN_INFO "Entering procfile_show");
    seq_printf(seq, "Value of counter: %ld\n", *tmp_v);
    return 0;
}

// Seq file ops
static struct seq_operations seq_ops = {
    .start = procfile_start,
    .next = procfile_next,
    .stop = procfile_stop,
    .show = procfile_show
};

// Opens the seq file for future operations, and returns a file descriptor to it
static int procfs_open(struct inode* inode, struct file* file)
{
    return seq_open(file, &seq_ops);
}

// File ops struct
static struct file_operations f_ops = {
    .owner = THIS_MODULE,
    .open = procfs_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release
};

static int __init hello(void)
{
    int i;
    printk(KERN_INFO "Hello world %d\n", data);
    // Create the proc file
    proc_file = proc_create(procfs_name, 0444, NULL, &f_ops);
    for (i = 0; i < strlen(MESSAGE); i++) {
        message[i] = MESSAGE[i];
    }
    msg_ptr = message;
    msg_len = strlen(message);
    printk(KERN_INFO "Message initialized to %s, length %ld\n", message, msg_len);
    return 0;
}

static void __exit goodbye(void)
{
    proc_remove(proc_file);
    printk(KERN_INFO "Goodbye world\n");
}

// fs_initcall(hello);
module_init(hello);
module_exit(goodbye);