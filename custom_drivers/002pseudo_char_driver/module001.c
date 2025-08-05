#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#define DEV_MEM_SIZE 512 // Size of the device memory

// Buffer to hold device data
char device_buff[DEV_MEM_SIZE];

// Device class
struct class *class_module001;

struct device *device_module001;

loff_t module001_lseek(struct file *filp, loff_t off, int whence);
loff_t module001_lseek(struct file *filp, loff_t off, int whence){
    pr_info("%s : lseek requested\n", __func__);
    pr_info("Current file position: %lld\n", filp->f_pos);
    loff_t temp;
    switch(whence){
        case SEEK_SET:
            if(off < 0 || off > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            filp->f_pos = off;
            break;
        case SEEK_CUR:
            temp = filp->f_pos + off;
            if(temp < 0 || temp > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            filp->f_pos = temp;
            break;
        case SEEK_END:
            temp = DEV_MEM_SIZE + off;
            if(temp < 0 || temp > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            filp->f_pos = temp;
            break;
        default:
            pr_err("%s : Invalid whence value\n", __func__);
            return -EINVAL; // Return error code for invalid whence
    }
    pr_info("New file position: %lld\n", filp->f_pos);
    return filp->f_pos; // Return the new file position
}

ssize_t module001_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t module001_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos){
    pr_info("%s : read requested for %zu byte\n", __func__, count);

    // 1. Ajust the file position
    pr_info("Current file position: %lld\n", *f_pos);
    if((*f_pos + count) > DEV_MEM_SIZE) {
        pr_info("%s : Adjusting count to fit within device memory\n", __func__);
        count = DEV_MEM_SIZE - *f_pos;
    }

    // 2. Copy data from kernel space to user space
    if (copy_to_user(buff, &device_buff[*f_pos], count)){
        pr_err("%s : Failed to copy data to user space\n", __func__);
        return -EFAULT; // Return error code
    }

    // 3. Update current file position
    *f_pos += count;
    pr_info("%s : Number file read %zu bytes\n", __func__, count);
    pr_info("Current file position after read: %lld\n", *f_pos);

    // 4. Return the number of bytes read
    return count;
}

ssize_t module001_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
ssize_t module001_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos){
    pr_info("%s : write requested for %zu byte\n", __func__, count);

     // 1. Ajust the file position
    pr_info("%s : Current file position: %lld\n", __func__, *f_pos);
    if((*f_pos + count) > DEV_MEM_SIZE) {
        count = DEV_MEM_SIZE - *f_pos;
    }

    if(!count) {
        pr_info("%s : No data to write\n", __func__);
        return -ENOMEM; // No data to write
    }

    // 2. Copy data from kernel space to user space
    if (copy_from_user(&device_buff[*f_pos], buff,count)){
        pr_err("%s : Failed to copy data to user space\n", __func__);
        return -EFAULT; // Return error code
    }

    // 3. Update current file position
    *f_pos += count;
    pr_info("%s : Number file write %zu bytes\n", __func__, count);
    pr_info("%s : Current file position after write: %lld\n", __func__,*f_pos);

    // 4. Return the number of bytes write
    return count;
}

int module001_open(struct inode *inode, struct file *filp);
int module001_open(struct inode *inode, struct file *filp){
    pr_info("%s : Device opened.\n", __func__);
    return 0; // Return 0 to indicate successful opening
}

int module001_release(struct inode *inode, struct file *filp);
int module001_release(struct inode *inode, struct file *filp){
    pr_info("%s : Device closed.\n", __func__);
    return 0; // Return 0 to indicate successful closing
}

// File operations of the driver
struct file_operations module001_fops = {
    .owner = THIS_MODULE,
    .llseek = module001_lseek,
    .read = module001_read,
    .write = module001_write,
    .open = module001_open,
    .release = module001_release,
};

static int __init __my_module_driver_init(void){
    printk(KERN_INFO "My module driver is being loaded.\n");

    int return_value;

    // 1. Dynamically allocate memory for the device
    return_value = alloc_chrdev_region(&dev_num, 0, 1, "devices_001");
    if(return_value < 0) {
        printk(KERN_ERR "Failed to allocate character device region.\n");
        goto out;
    }
    pr_info("%s : Device created with major: %d, minor: %d\n", __func__,MAJOR(dev_num), MINOR(dev_num));

    // 2. Initialize the cdev structure
    cdev_init(&cdev_module001, &module001_fops);

    // 3. Add the cdev to the system with VFS
    cdev_module001.owner = THIS_MODULE;
    return_value = cdev_add(&cdev_module001, dev_num, 1);
    if (return_value < 0) {
        printk(KERN_ERR "Failed to add cdev.\n");
        goto unreg_chrdev;
    }

    // 4. Create a device class
    class_module001 = class_create("my_device_class");
    if(IS_ERR(class_module001)) {
        printk(KERN_ERR "Failed to create device class.\n");
        return_value = PTR_ERR(class_module001);
        goto cdev_del;
    }
    device_module001 = device_create(class_module001, NULL, dev_num, NULL, "my_device");
    if(IS_ERR(device_module001)) {
        printk(KERN_ERR "Failed to create device.\n");
        return_value = PTR_ERR(device_module001);
        goto class_destroy;
    }
    pr_info("Module initialized with device: %s\n", "my_device");

    return 0; // Return 0 to indicate successful loading

class_destroy:
    class_destroy(class_module001);
cdev_del:
    cdev_del(&cdev_module001);
unreg_chrdev:
    printk(KERN_ERR "Failed to add cdev.\n");
    unregister_chrdev_region(dev_num, 1);
out: 
    printk(KERN_ERR "Module initialization failed.\n");
    return return_value; // Return error code
}

static void __exit __my_module_driver_exit(void){
    // Decontruct the device and free resources
    device_destroy(class_module001, dev_num);
    class_destroy(class_module001);
    cdev_del(&cdev_module001);
    unregister_chrdev_region(dev_num, 1);
    pr_info("%s : Module exited and resources freed.\n", __func__);
    printk(KERN_INFO "My module driver is being unloaded.\n");
}



module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("002");
MODULE_DESCRIPTION("A simple module driver for a pseudo character device");
MODULE_INFO(boarder, "Module001");
