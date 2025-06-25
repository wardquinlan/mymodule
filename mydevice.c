#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>// Module metadata

MODULE_AUTHOR("Ward Quinlan");
MODULE_DESCRIPTION("My first kernel device");
MODULE_LICENSE("GPL");// Custom init and exit methods

// Based on sample code taken from:
//
// https://sysprog21.github.io/lkmpg/#the-fileoperations-structure

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

static struct file_operations chardev_fops = { 
    .read = custom_read, 
}; 

static int __init custom_init(void)
{
	printk(KERN_INFO "My kernel device loaded\n");
	return 0;
}

static void __exit custom_exit(void)
{
	printk(KERN_INFO "My kernel device unloaded\n");
}

module_init(custom_init);
module_exit(custom_exit);

