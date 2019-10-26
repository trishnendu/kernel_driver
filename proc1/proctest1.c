#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

ssize_t read_procent(struct file *fp, char *buf, size_t buf_size, loff_t *offp);
void collect_pslist(void);
void collect_pslist1(void);

struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = read_procent,
};

char *msg = NULL;
int msg_len = 0;
int unread_len = 0;
int stats_collected = 0;

ssize_t read_procent(struct file *fp, char *buf, size_t buf_size, loff_t *offp){
    int byte_read = 0;
    if(!stats_collected){
        collect_pslist1();
        stats_collected = 1;
        unread_len = msg_len;
    }
    if(unread_len){
        int blk_size = unread_len < buf_size ? unread_len : buf_size; 
        byte_read = blk_size - copy_to_user(buf, msg + msg_len - unread_len, blk_size);
        unread_len -= byte_read;
    }
    if(!byte_read){
        stats_collected = 0;
    }  
    return byte_read;
}

void collect_pslist(void){
    struct task_struct *task_list;
    msg_len = 0;

    if(!msg)
        msg = kmalloc(1024*1024, GFP_KERNEL);
    
    for_each_process(task_list){
        msg_len += sprintf(msg+msg_len, "%s %d\n", task_list->comm, task_list->pid);
	    if(msg_len > 1000*1000)	
            break;
    }
    printk(KERN_ERR "collected %d byte of ps info\n", msg_len);
}

void __collect_pslist1(struct task_struct *root){
    if(msg_len > 1000 * 1000)
        return;
    if(root){
        struct list_head *task_list = NULL;
        msg_len += sprintf(msg+msg_len, "%d, %d\n", root->parent->pid, root->pid);
        list_for_each(task_list, &root->children){
            struct task_struct *task = list_entry(task_list, struct task_struct, sibling);
            __collect_pslist1(task);
        }
    }
}

void collect_pslist1(void){
    struct task_struct *root = current;
    msg_len = 0;
    if(!msg)
        msg = kmalloc(1024*1024, GFP_KERNEL);
    
    printk(KERN_ERR "starting traversal from current pid %d\n", root->pid);
    for( ; root->parent != root; root = root->parent);
    __collect_pslist1(root);
}

void collect_pslist2(void){
    
}

int myinit(void) {
    printk(KERN_ERR "loading proctest1 module\n");
    proc_create("ps_list", 0, NULL, &proc_fops);
    return 0;
}

void mydeinit(void) {
    remove_proc_entry("ps_list", NULL);
    printk(KERN_ERR "unloading proctest1 module\n");
}

MODULE_LICENSE("GPL");
module_init(myinit);
module_exit(mydeinit);
