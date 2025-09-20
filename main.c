#include "mydrv.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);

int major_number;
dev_t dev;
struct class* mydrv_class = NULL;
struct device* mydrv_device = NULL;
struct cdev mydrv_cdev;
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mydrv_open,
    .release = mydrv_release,
    .read = mydrv_read,
    .write = mydrv_write,
    .llseek = my_llseek,
    .mmap = mydrv_mmap,
};

static int __init mydrv_init(void)
{
    printk(KERN_INFO "mydrv: Initializing the mydrv driver\n");
    dev = MKDEV(major_number, 0);
    uint8_t ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "mydrv: Failed to allocate major number\n");
        return ret;
    }
    printk(KERN_INFO "mydrv: Registered with major number %d\n", major_number);

    cdev_init(&mydrv_cdev, &fops); mydrv_cdev.owner = THIS_MODULE;
    cdev_add(&mydrv_cdev, dev, 1);

    mydrv_class = class_create(THIS_MODULE, CLASS_NAME);
    mydrv_device = device_create(mydrv_class, NULL, dev, NULL, DEVICE_NAME);
    printk(KERN_INFO "mydrv: Device created successfully. Use /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit mydrv_exit(void)
{
    cdev_del(&mydrv_cdev);
    device_destroy(mydrv_class, dev);
    class_destroy(mydrv_class);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "mydrv: Driver cleanup complete\n");
}

module_init(mydrv_init);
module_exit(mydrv_exit);
