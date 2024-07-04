#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static struct dentry *dir = 0;
char list[]="local_irq_save_restore: call local_irq_save, sleep for the number of ns, then call local_irq_restore.\n ";

static ssize_t list_read(struct file *f, char *buffer,
		size_t len, loff_t *offset)
{
  
  return simple_read_from_buffer(buffer, len, offset, list, sizeof(list)/sizeof(char));
}

static ssize_t irq_write(struct file *f, const char *buffer,
		size_t len, loff_t *offset)
{
  ssize_t ret = len;
  
  unsigned long long sleep_counts;
  sleep_counts = kstrtoull_from_user(buffer, len, 10, &sleep_counts);
  
  if (sleep_counts <= 0 || sleep_counts > 1000) {
    printk(KERN_ALERT "kfexport: sleep_counts should be greater than 0, and less than 1000");
  } else {
    unsigned long flags;
    local_irq_save(flags);
    msleep(sleep_counts);
    local_irq_restore(flags);
  }
  
  return ret;
}

const struct file_operations list_fops = {
  .owner = THIS_MODULE,
  .read = list_read,
};

const struct file_operations local_irq_save_restore_fops = {
  .owner = THIS_MODULE,
  .write = irq_write,
};

int init_module(void)
{
  struct dentry *junk;

  dir = debugfs_create_dir("kfexport", 0);
  if (!dir) {
    printk(KERN_ALERT "kfexport: failed to create /sys/kernel/debug/fsexport\n");
    return -1;
  }

  junk = debugfs_create_file(
			     "list",
			     0444,
			     dir,
			     NULL,
			     &list_fops);
  if (!junk) {
    printk(KERN_ALERT "kfexport: failed to create /sys/kernel/debug/fsexport/list\n");
    return -1;
  }

  junk = debugfs_create_file("local_irq_save_restore", 0222, dir,NULL,&local_irq_save_restore_fops);
  if (!junk) {
    printk(KERN_ALERT "kfexport: failed to create /sys/kernel/debug/fsexport/local_irq_save_restore\n");
    return -1;
  }

  return 0;
}

void cleanup_module(void)
{
  debugfs_remove_recursive(dir);
}
