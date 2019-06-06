/**
 *  procfs4.c -  create a "file" in /proc
 * 	This program uses the seq_file library to manage the /proc file.
 *
 */

#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#include <linux/slab.h>

#define PROC_NAME "iter"

MODULE_AUTHOR("Philippe Reynes");
MODULE_LICENSE("GPL");

static struct proc_dir_entry* entry;

/**
 * This function is called at the beginning of a sequence.
 * ie, when:
 *	- the /proc file is read (first time)
 *	- after the function stop (end of sequence)
 *
 */
static void* my_seq_start(struct seq_file* s, loff_t* pos)
{
    unsigned long* counter = kmalloc(sizeof(unsigned long), GFP_KERNEL);
    /* beginning a new sequence ? */
    if (*pos == 0) {
        /* yes => return a non null value to begin the sequence */
        *counter = *pos;
        return counter;
    } else {
        /* no => it's the end of the sequence, return end to stop reading */
        *pos = 0;
        return NULL;
    }
}

/**
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 *
 */
static void* my_seq_next(struct seq_file* s, void* v, loff_t* pos)
{
    unsigned long* tmp_v = (unsigned long*)v;
    (*tmp_v)++;
    (*pos)++;
    if (*tmp_v == 5) {
        return NULL;
    } else {
        return tmp_v;
    }
}

/**
 * This function is called at the end of a sequence
 * 
 */
static void my_seq_stop(struct seq_file* s, void* v)
{
    kfree(v);
}

/**
 * This function is called for each "step" of a sequence
 *
 */
static int my_seq_show(struct seq_file* s, void* v)
{
    loff_t* spos = (loff_t*)v;

    seq_printf(s, "%Ld\n", *spos);
    return 0;
}

/**
 * This structure gather "function" to manage the sequence
 *
 */
static struct seq_operations my_seq_ops = {
    .start = my_seq_start,
    .next = my_seq_next,
    .stop = my_seq_stop,
    .show = my_seq_show
};

/**
 * This function is called when the /proc file is open.
 *
 */
static int my_open(struct inode* inode, struct file* file)
{
    return seq_open(file, &my_seq_ops);
};

/**
 * This structure gather "function" that manage the /proc file
 *
 */
static struct file_operations my_file_ops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release
};

/**
 * This function is called when the module is loaded
 *
 */
int init_module(void)
{

    entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);

    return 0;
}

/**
 * This function is called when the module is unloaded.
 *
 */
void cleanup_module(void)
{
    proc_remove(entry);
}