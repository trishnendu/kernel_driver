#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define __DEVICE_NAME "mychrdev2"
#define BUF_LEN 64

static int chrdev_open(struct inode*, struct file*);
static ssize_t chrdev_read(struct file*, char *, size_t, loff_t *);
static ssize_t chrdev_write(struct file*, const char *, size_t, loff_t *);

static int Major;
static struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = chrdev_open,
	.read = chrdev_read,
	.write = chrdev_write,
};

static char msg[BUF_LEN];
static int byte_read, byte_write;

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
	byte_read = byte_write = 0;
	return 0; 
}

static ssize_t chrdev_read(struct file *file, char *buf, size_t len, loff_t *offset){
	int read_cnt = 0;
	char *msg_ptr = msg + byte_read;
	while (len-- && *msg_ptr){
		put_user(*(msg_ptr++), buf++);
		read_cnt++;
	}
	byte_read += read_cnt;
	return read_cnt;
}

static ssize_t chrdev_write(struct file *file, const char *buf, size_t len, loff_t *offset){
	int write_cnt = 0;
	char *msg_ptr = msg + byte_write;
	if (len > BUF_LEN){
		printk(KERN_ALERT "message too long \n");
		return 0;
	}
	while(len--){
		get_user(*(msg_ptr++), buf++);	
		write_cnt++;
	}
	byte_write += write_cnt;
	*msg_ptr = (char)0;
	return write_cnt;
}

module_init(init);
module_exit(cleanup);
