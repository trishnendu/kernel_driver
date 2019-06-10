#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#define __DEVICE_NAME "mychrdev"

int init_module(void);
void cleanup_module(void);


static int Major;
static struct file_operations fops;

int init(void){
	Major = register_chrdev(0, __DEVICE_NAME, &fops);
	if (Major < 0){
		printk(KERN_ALERT "falied to register charecter device %s\n", __DEVICE_NAME);
		return Major;
	}
	printk(KERN_INFO "successfully registered charecter device %s\n", __DEVICE_NAME); 
	return 0;
}

void cleanup(void){
	printk(KERNEL_INFO "unregistering charecter device %s\n", __DEVICE_NAME);
	unregister_chrdev(Major, __DEVICE_NAME);
}

module_init(init);
module_exit(cleanup);
