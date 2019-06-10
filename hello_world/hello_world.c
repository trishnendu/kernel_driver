#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

int hello_world_init(void){
	printk(KERN_INFO "Hello World init");
	return 0;
}

int hello_world_clean(void){
	printk(KERN_INFO "cleaning world");
	return 0;
}

module_init(hello_world_init);
module_exit(hello_world_clean);
