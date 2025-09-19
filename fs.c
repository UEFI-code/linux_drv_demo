#include "mydrv.h"

/* Global message to be returned when device is read */
char message[] = "hello\n";
int message_len = 6;

/* Device open function */
int mydrv_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device opened\n");
    return 0;
}

/* Device release function */
int mydrv_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device closed\n");
    return 0;
}

/* Device read function */
ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    if (*offset >= message_len) return 0;

    if (*offset + len > message_len)
        len = message_len - *offset;
    
    if (copy_to_user(buffer, message + *offset, len)) {
        return -EFAULT;
    }
    
    *offset += len;
    printk(KERN_INFO "mydrv: Sent %d bytes to user\n", len);
    return len;
}

ssize_t mydrv_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "mydrv: Write operation not supported\n");
    return -EINVAL;
}