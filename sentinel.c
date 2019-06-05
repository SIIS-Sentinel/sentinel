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
#include <linux/swap.h>
#include <linux/sysinfo.h>

#define procfs_name "sentinel"
#define MESSAGE "Welcome to Sentinel\n\0"
#define BUF_SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrien Cosson");
MODULE_DESCRIPTION("Hardware monitoring module");
MODULE_VERSION("0.3");

static int __initdata data = 3;
struct proc_dir_entry* proc_file;
static char message[BUF_SIZE];
static char* msg_ptr;
static long int msg_len;
static int max_iterations = 10;

ssize_t procfile_read(struct file* file, char __user* buffer, size_t len, loff_t* offset)
{
    // char __user* ptr;
    // unsigned long retval;
    int bytes_read = 0;
    printk(KERN_INFO "Read handler of Sentinel called\n");
    printk(KERN_INFO "Length: %ld, offset: %lld\n", len, *offset);

    // Reset the message counter if necessary
    if (*msg_ptr == 0) {
        msg_ptr = message;
    }

    // Verify if we have sent the messahe enough times
    if ((*offset) / msg_len == max_iterations) {
        return 0;
    }

    // Copy the message once
    while (len && *msg_ptr) {
        put_user(*(msg_ptr)++, buffer++);
        len--;
        bytes_read++;
        (*offset)++;
    }

    printk(KERN_INFO "Number of bytes copied: %d", bytes_read);
    return bytes_read;
}

ssize_t procfile_write(struct file* file, const char __user* buffer, size_t len, loff_t* offset)
{
    printk(KERN_INFO "Write handler of Sentinel called\n");
    printk(KERN_ALERT "This device does not support write operations\n");
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

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = procfile_read,
    .write = procfile_write
};

static int __init hello(void)
{
    int i;
    printk(KERN_INFO "Hello world %d\n", data);
    // Create the proc file
    proc_file = proc_create(procfs_name, 0444, NULL, &fops);
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