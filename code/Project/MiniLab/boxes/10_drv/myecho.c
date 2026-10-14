/*
 * myecho.c - 教学用字符设备驱动
 * 对应文档：§18 盒 ⑩
 *
 * 用法：
 *   make
 *   sudo insmod myecho.ko
 *   echo hi > /dev/myecho && cat /dev/myecho
 *   sudo rmmod myecho
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/mutex.h>

#define DEVICE_NAME "myecho"
#define CLASS_NAME  "mlab"
#define BUF_SIZE    4096

static dev_t          dev_num;
static struct cdev    my_cdev;
static struct class  *my_class;
static struct device *my_device;

static char   *buffer;
static size_t  buffer_len;
static DEFINE_MUTEX(buf_lock);

static int myecho_open(struct inode *inode, struct file *file) {
    pr_info("myecho: open by pid=%d\n", current->pid);
    return 0;
}

static int myecho_release(struct inode *inode, struct file *file) {
    pr_info("myecho: close by pid=%d\n", current->pid);
    return 0;
}

static ssize_t myecho_read(struct file *file, char __user *ubuf,
                           size_t len, loff_t *off) {
    ssize_t ret;
    mutex_lock(&buf_lock);
    if (*off >= (loff_t)buffer_len) { ret = 0; goto out; }
    {
        size_t to_copy = min(len, buffer_len - (size_t)*off);
        if (copy_to_user(ubuf, buffer + *off, to_copy)) { ret = -EFAULT; goto out; }
        *off += to_copy;
        ret = to_copy;
    }
out:
    mutex_unlock(&buf_lock);
    return ret;
}

static ssize_t myecho_write(struct file *file, const char __user *ubuf,
                            size_t len, loff_t *off) {
    ssize_t ret;
    mutex_lock(&buf_lock);
    {
        size_t to_copy = min(len, (size_t)BUF_SIZE);
        if (copy_from_user(buffer, ubuf, to_copy)) { ret = -EFAULT; goto out; }
        buffer_len = to_copy;
        ret = to_copy;
    }
out:
    mutex_unlock(&buf_lock);
    return ret;
}

static const struct file_operations myecho_fops = {
    .owner   = THIS_MODULE,
    .open    = myecho_open,
    .release = myecho_release,
    .read    = myecho_read,
    .write   = myecho_write,
    .llseek  = default_llseek,
};

static int __init myecho_init(void) {
    int ret;

    buffer = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!buffer) return -ENOMEM;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) { pr_err("alloc_chrdev_region\n"); goto err_buf; }

    cdev_init(&my_cdev, &myecho_fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) { pr_err("cdev_add\n"); goto err_region; }

    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) { ret = PTR_ERR(my_class); goto err_cdev; }
    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) { ret = PTR_ERR(my_device); goto err_class; }

    pr_info("myecho: loaded. /dev/%s  major=%d minor=%d\n",
            DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
    return 0;

err_class:  class_destroy(my_class);
err_cdev:   cdev_del(&my_cdev);
err_region: unregister_chrdev_region(dev_num, 1);
err_buf:    kfree(buffer);
    return ret;
}

static void __exit myecho_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    kfree(buffer);
    pr_info("myecho: unloaded\n");
}

module_init(myecho_init);
module_exit(myecho_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MiniLab");
MODULE_DESCRIPTION("A tiny echo char device for teaching");
