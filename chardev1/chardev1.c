#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define __DEVICE_NAME "mychrdev1"
#define BUF_LEN 64

static int chrdev_open(struct inode*, struct file*);
static ssize_t chrdev_read(struct file*, char *, size_t, loff_t *);

static int Major;
static struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = chrdev_open,
	.read = chrdev_read,
	//.write = chrdev_write,
};
//static int Device_ref = 0;

static char msg[BUF_LEN];
static char *msg_ptr;

int init(void){
	Major = register_chrdev(0, __DEVICE_NAME, &fop);

	if (Major < 0){
		printk(KERN_ALERT "chrdev register failed for %s\n", __DEVICE_NAME);
		return Major;
	}
	printk(KERN_INFO "chrdev registration successful for Device Name = %s, Major = %d\n", __DEVICE_NAME, Major);
	return 0;
}

void cleanup(void){
	printk(KERN_INFO "chrdev unregistering for %s\n", __DEVICE_NAME);
	unregister_chrdev(Major, __DEVICE_NAME);
}

static int chrdev_open(struct inode *inode, struct file *file){
	static int cnt = 1;
	/*
 	if (Device_ref){
		printk(KERN_ALERT "device file is already opened for %s\n", __DEVICE_NAME);
		return -EBUSY;
	}
	Device_ref++;
	*/
	sprintf(msg, "chrdev %s hasbeen opened for %d times till now\n", __DEVICE_NAME, cnt++);
	msg_ptr = msg;
	//try_module_get(THIS_MODULE);
	return 0; 
}

/*
static int chrdev_release(struct inode *inode, struct file *file){
	Device_ref--;
	module_put(THIS_MODULE);
	return 0;
}
*/
static ssize_t chrdev_read(struct file *file, char *buf, size_t len, loff_t *offset){
	int byte_read = 0;
	if (*msg_ptr == 0){
		return 0;
	}
	while (len && *msg_ptr){
		put_user(*(msg_ptr++), buf++);
		len--;
		byte_read++;
	}
	return byte_read;
}

module_init(init);
module_exit(cleanup);
