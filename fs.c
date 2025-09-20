#include "mydrv.h"

struct page *page = 0;
uint64_t pfn = 0;
#define page_count 1 // 4KB
char *buf = 0;
#define buf_size (page_count * PAGE_SIZE)

int mydrv_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device opened\n");
    if (!buf) {
        page = alloc_pages(GFP_KERNEL, page_count);
        pfn = page_to_pfn(page);
        buf = page_address(page);
        pr_alert("mydrv: page=0x%llx, pfn=0x%llx, buf=0x%llx\n", page, pfn, buf);
        memset(buf, 0, buf_size);
        snprintf(buf, buf_size, "hello\n");
    }
    return 0;
}

int mydrv_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mydrv: Device closed\n");
    return 0;
}

ssize_t mydrv_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    if (*offset >= buf_size) return 0;

    if (*offset + len > buf_size)
        len = buf_size - *offset;

    if (copy_to_user(buffer, buf + *offset, len)) {
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
            file->f_pos = buf_size + offset;
            break;
        default:
            return -EINVAL;
    }
    return file->f_pos;
}

int mydrv_mmap(struct file *filp, struct vm_area_struct *vma)
{
    pr_alert("mydrv: mmap requested for pfn 0x%llx\n", pfn);
    uint64_t vsize = vma->vm_end - vma->vm_start;
    if (vsize > buf_size) return -EINVAL;
    remap_pfn_range(vma, vma->vm_start, pfn, vsize, vma->vm_page_prot);
    return 0;
}