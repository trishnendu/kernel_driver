#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

int read_procent(struct file *fp, char *buf, size_t buf_size, loff_t *offp);
void collect_pslist();

struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = read_procent,
};

char *msg = 0;
int msg_len = 0;
int unread_len = 0;

int read_procent(struct file *fp, char *buf, size_t buf_size, loff_t *offp){
    int byte_read = 0;
    if(unread_len){
        int blk_size = unread_len < buf_size ? unread_len : buf_size; 
        byte_read = blk_size - copy_to_user(buf, msg + msg_len - unread_len, blk_size);
        unread_len -= byte_read;
    }
    if(!byte_read)  
        unread_len = msg_len;
    return byte_read;
}

void collect_pslist(){
    struct task_struct *task_list;

    if(!msg)
        msg = kmalloc(1024*1024, GFP_KERNEL);
    
    for_each_process(task_list){
        msg_len += sprintf(msg+msg_len, "%s %d\n", task_list->comm, task_list->pid);
	    if(msg_len > 1000*1000)	
            break;
    }
    unread_len = msg_len;
    printk(KERN_ERR "collected %d byte of ps info\n", unread_len);
}

int myinit(void) {
    printk(KERN_ERR "loading proctest1 module\n");
    proc_create("ps_list", 0, NULL, &proc_fops);
    collect_pslist();
    return 0;
}

void mydeinit(void) {
    printk(KERN_ERR "unloading proctest1 module\n");
    remove_proc_entry("ps_list", NULL);
}

MODULE_LICENSE("GPL");
module_init(myinit);
module_exit(mydeinit);
