#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/* Driver information */
#define DEVICE_NAME "mydrv"
#define CLASS_NAME "mydrv_class"
#define DRIVER_AUTHOR "SuperHacker UEFI"
#define DRIVER_DESC "A simple cdev drv to echo hello"
#define DRIVER_VERSION "1.0"

extern int mydrv_open(struct inode *inode, struct file *file);
extern int mydrv_release(struct inode *inode, struct file *file);
extern ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset);
extern ssize_t mydrv_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset);