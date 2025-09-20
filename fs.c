#include "mydrv.h"

/* Global message to be returned when device is read */
char message[] = "hello\n";

int mydrv_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device opened\n");
    return 0;
}

int mydrv_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device closed\n");
    return 0;
}

ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    if (*offset >= sizeof(message)) return 0;

    if (*offset + len > sizeof(message))
        len = sizeof(message) - *offset;

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

loff_t my_llseek(struct file *file, loff_t offset, int whence) {
    switch (whence) {
        case SEEK_SET:
            file->f_pos = offset;
            break;
        case SEEK_CUR:
            file->f_pos += offset;
            break;
        case SEEK_END:
            file->f_pos = sizeof(message) + offset;
            break;
        default:
            return -EINVAL;
    }
    return file->f_pos;
}