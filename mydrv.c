#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mydrv"
#define CLASS_NAME "mydrv_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SuperHacker UEFI");
MODULE_DESCRIPTION("A simple cdev");
MODULE_VERSION("1.0");

static int major_number;
static struct class* mydrv_class = NULL;
static struct device* mydrv_device = NULL;
static struct cdev mydrv_cdev;

// Message to be returned when device is read
static char message[] = "hello\n";
static int message_len = 6;

// Function prototypes
static int mydrv_open(struct inode *inode, struct file *file);
static int mydrv_release(struct inode *inode, struct file *file);
static ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset);
static ssize_t mydrv_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset);

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mydrv_open,
    .release = mydrv_release,
    .read = mydrv_read,
    .write = mydrv_write,
};

// Device open function
static int mydrv_open(struct inode *inode, struct file *file)
{
    pr_alert("mydrv: Device opened\n");
    return 0;
}

// Device release function
static int mydrv_release(struct inode *inode, struct file *file)
{
    pr_alert("mydrv: Device closed\n");
    return 0;
}

// Device read function
static ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    // Check if we've already read everything
    if (*offset >= message_len)
        return 0;
    
    // Calculate how many bytes to read
    if (*offset + len > message_len)
        len = message_len - *offset;
    
    // Copy data to user space
    if (copy_to_user(buffer, message + *offset, len)) {
        return -EFAULT;
    }
    
    *offset += len;
    pr_alert("mydrv: Sent %d bytes to user\n", len);
    return len;
}

// Device write function
static ssize_t mydrv_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "mydrv: Write operation not supported\n");
    return -EINVAL;
}

// Module initialization function
static int __init mydrv_init(void)
{
    dev_t dev;
    int ret;

    pr_alert("mydrv: Initializing the mydrv driver\n");

    // Allocate a major number dynamically
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_alert("mydrv: Failed to allocate major number\n");
        return ret;
    }
    major_number = MAJOR(dev);
    pr_alert("mydrv: Registered with major number %d\n", major_number);

    // Initialize cdev structure
    cdev_init(&mydrv_cdev, &fops);
    mydrv_cdev.owner = THIS_MODULE;
    
    // Add cdev to the system
    ret = cdev_add(&mydrv_cdev, dev, 1);
    if (ret < 0) {
        pr_alert("mydrv: Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return ret;
    }
    
    // Create device class
    mydrv_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mydrv_class)) {
        pr_alert("mydrv: Failed to create device class\n");
        cdev_del(&mydrv_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(mydrv_class);
    }
    
    // Create device file
    mydrv_device = device_create(mydrv_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(mydrv_device)) {
        pr_alert("mydrv: Failed to create device\n");
        class_destroy(mydrv_class);
        cdev_del(&mydrv_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(mydrv_device);
    }
    
    pr_alert("mydrv: Device created successfully. Use /dev/%s\n", DEVICE_NAME);
    return 0;
}

// Module cleanup function
static void __exit mydrv_exit(void)
{
    dev_t dev = MKDEV(major_number, 0);

    pr_alert("mydrv: Cleaning up the mydrv driver\n");

    // Remove device
    device_destroy(mydrv_class, dev);
    
    // Remove class
    class_destroy(mydrv_class);
    
    // Remove cdev
    cdev_del(&mydrv_cdev);
    
    // Unregister major number
    unregister_chrdev_region(dev, 1);
    
    printk(KERN_INFO "mydrv: Driver cleanup complete\n");
}

module_init(mydrv_init);
module_exit(mydrv_exit);
