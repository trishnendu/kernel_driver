#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#define __DEVICE_NAME "mychrdev2"
#define BUF_LEN 64

static int chrdev_open(struct inode*, struct file*);
static ssize_t chrdev_bulkread(struct file*, char *, size_t, loff_t *);
static ssize_t chrdev_bulkwrite(struct file*, const char *, size_t, loff_t *);

static int Major;
static struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = chrdev_open,
	.read = chrdev_bulkread,
	.write = chrdev_bulkwrite,
};

static char msg[BUF_LEN];
static int msg_len;
static int byte_read, byte_write;

int init(void) {
	Major = register_chrdev(0, __DEVICE_NAME, &fop);

	if (Major < 0) {
		printk(KERN_ALERT "chrdev register failed for %s\n", __DEVICE_NAME);
		return Major;
	}
	printk(KERN_INFO "chrdev registration successful for Device Name = %s, Major = %d\n", __DEVICE_NAME, Major);
	return 0;
}

void cleanup(void) {
	printk(KERN_INFO "chrdev unregistering for %s\n", __DEVICE_NAME);
	unregister_chrdev(Major, __DEVICE_NAME);
}

static int chrdev_open(struct inode *inode, struct file *file) {
	byte_read = byte_write = 0;
	return 0; 
}

static ssize_t chrdev_bulkread(struct  file *file, char *buf, size_t len, loff_t *offset) {
	int not_copied = 0;
	int to_be_copied;
	
	to_be_copied = min(msg_len - byte_read, len);
	not_copied = copy_to_user(buf, msg + byte_read, to_be_copied);
	byte_read += to_be_copied - not_copied;

	return to_be_copied - not_copied;
};

static ssize_t chrdev_bulkwrite(struct  file *file, const char *buf, size_t len, loff_t *offset) {
	int not_copied = 0;

	if (len > BUF_LEN - byte_write) {
		printk(KERN_ALERT "message too long \n");
		return 0;
	}

	not_copied = copy_from_user(msg + byte_write, buf, len);
	msg_len = byte_write += len - not_copied;

	return len - not_copied;
};


module_init(init);
module_exit(cleanup);
