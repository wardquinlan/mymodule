#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/proc_fs.h>// Module metadata

MODULE_AUTHOR("Ward Quinlan");
MODULE_DESCRIPTION("My first kernel device");
MODULE_LICENSE("GPL");// Custom init and exit methods

// Based on sample code taken from:
//
// https://sysprog21.github.io/lkmpg/#the-fileoperations-structure

#define DEVICE_NAME "mydevice"
static int major;
static struct class *cls;

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
	printk(KERN_INFO "registering character device\n");
	// passing 0 as the major number means ask it to dynamically assign a major device number
	major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
	printk(KERN_INFO "assigned major number %d\n", major);

	// allocate the class
	printk(KERN_INFO "creating class\n");
	cls = class_create(DEVICE_NAME);

	printk(KERN_INFO "creating device\n");
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

	printk(KERN_INFO "my kernel device created on /dev/%s\n", DEVICE_NAME);
	return 0;
}

static void __exit custom_exit(void)
{
	printk(KERN_INFO "destroying device\n");
	device_destroy(cls, MKDEV(major, 0)); 

	printk(KERN_INFO "destroying class\n");
	class_destroy(cls); 

	printk(KERN_INFO "my kernel device unloaded\n");
}

module_init(custom_init);
module_exit(custom_exit);

