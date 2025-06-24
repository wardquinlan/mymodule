#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>// Module metadata

MODULE_AUTHOR("Ward Quinlan");
MODULE_DESCRIPTION("My first kernel module");
MODULE_LICENSE("GPL");// Custom init and exit methods

// Based on sample code taken from:
//
// https://medium.com/dvt-engineering/how-to-write-your-first-linux-kernel-module-cf284408beeb
// https://stackoverflow.com/questions/8516021/proc-create-example-for-kernel-module

static struct proc_dir_entry* proc_entry;

static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
	printk(KERN_INFO "calling custom read method\n");
	printk(KERN_INFO "user buffer size=%ld\n", count);
	char greeting[] = "greetings\n";
	int greeting_length = strlen(greeting);
	if (*offset > 0)
	{
                printk(KERN_INFO "exiting read\n");
		return 0;
	}
	int ret = copy_to_user(user_buffer, greeting, greeting_length);
	printk(KERN_INFO "copy_to_user returned %d\n", ret);
	*offset = greeting_length;
	return greeting_length;
}

static struct proc_ops pops =
{
	.proc_read = custom_read
};

static int __init custom_init(void)
{
	printk(KERN_INFO "My kernel module loaded\n");
	proc_entry = proc_create("mymodule", 0666, NULL, &pops);
	return 0;
}

static void __exit custom_exit(void)
{
	proc_remove(proc_entry);
	printk(KERN_INFO "My kernel module unloaded\n");
}

module_init(custom_init);
module_exit(custom_exit);

