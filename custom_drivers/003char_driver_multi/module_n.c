#include"module_n.h"

struct module_private_data_driver module_private_data = {
    .total_devices = NO_OF_DEVICES,
    .module_data = {
        [0] = {
            .buff = device_buff_mod1,
            .size = MEM_MAX_MOD_1,
            .serial_number = "N001",
            .perm = RDONLY // Read permissions
        },
        [1] = {
            .buff = device_buff_mod2,
            .size = MEM_MAX_MOD_2,
            .serial_number = "N002",
            .perm = WRONLY // Write permissions
        },
        [2] = {
            .buff = device_buff_mod3,
            .size = MEM_MAX_MOD_3,
            .serial_number = "N003",
            .perm = RDWR // Read Write permissions
        },
        [3] = {
            .buff = device_buff_mod4,
            .size = MEM_MAX_MOD_4,
            .serial_number = "N004",
            .perm = RDWR // Read Write permissions
        }
    },
};

loff_t module001_lseek(struct file *flip, loff_t off, int whence){
    pr_info("%s : lseek requested\n", __func__);
    pr_info("Current file position: %lld\n", flip->f_pos);

        // Get the private data from the file structure
    struct module_private_data *module_data = (struct module_private_data*)flip->private_data;
    int DEV_MEM_SIZE = module_data->size; // Get the device memory size

    loff_t temp;
    switch(whence){
        case SEEK_SET:
            if(off < 0 || off > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            flip->f_pos = off;
            break;
        case SEEK_CUR:
            temp = flip->f_pos + off;
            if(temp < 0 || temp > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            flip->f_pos = temp;
            break;
        case SEEK_END:
            temp = DEV_MEM_SIZE + off;
            if(temp < 0 || temp > DEV_MEM_SIZE) {
                pr_err("%s : Invalid offset value\n", __func__);
                return -EINVAL; // Return error code for invalid offset
            }
            flip->f_pos = temp;
            break;
        default:
            pr_err("%s : Invalid whence value\n", __func__);
            return -EINVAL; // Return error code for invalid whence
    }
    pr_info("New file position: %lld\n", flip->f_pos);
    return flip->f_pos; // Return the new file position
}

ssize_t module001_read(struct file *flip, char __user *buff, size_t count, loff_t *f_pos){
    pr_info("%s : read requested for %zu byte\n", __func__, count);

    // Get the private data from the file structure
    struct module_private_data *module_data = (struct module_private_data*)flip->private_data;

    int DEV_MEM_SIZE = module_data->size; // Get the device memory size


    // 1. Ajust the file position
    pr_info("Current file position: %lld\n", *f_pos);
    if((*f_pos + count) > DEV_MEM_SIZE) {
        pr_info("%s : Adjusting count to fit within device memory\n", __func__);
        count = DEV_MEM_SIZE - *f_pos;
    }

    // 2. Copy data from kernel space to user space
    if (copy_to_user(buff, module_data->buff + (*f_pos), count)){
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

ssize_t module001_write(struct file *flip, const char __user *buff, size_t count, loff_t *f_pos){
    pr_info("%s : write requested for %zu byte\n", __func__, count);

    // Get the private data from the file structure
    struct module_private_data *module_data = (struct module_private_data*)flip->private_data;

    int DEV_MEM_SIZE = module_data->size; // Get the device memory size

     // 1. Ajust the file position
    pr_info("%s : Current file position: %lld\n", __func__, *f_pos);
    if((*f_pos + count) > DEV_MEM_SIZE) {
        count = DEV_MEM_SIZE - *f_pos;
    }

    if(!count) {
        pr_info("%s : No space left to write\n", __func__);
        return -ENOMEM;
    }

    // 2. Copy data from kernel space to user space
    if (copy_from_user(module_data->buff + (*f_pos), buff,count)){
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

int Check_Permissions(int perm, int minor_n, int flags){
    pr_info("%s : Checking permissions for minor number: %d\n", __func__, minor_n);
    
    if(perm == RDWR){
        return 0;
    }

    pr_info("%s : flags: %d, RFMODE_READ: %d, FMODE_WRITE: %d\n", __func__, flags,FMODE_READ , FMODE_WRITE);
    // Read permissions
    if((perm == RDONLY) && ((flags & FMODE_READ) && !(flags & FMODE_WRITE))){
        pr_info("%s : Read permissions granted for minor number: %d\n", __func__, minor_n);
        return 0;
    }

    // Write permissions
    if((perm == WRONLY) && (!(flags & FMODE_READ) && (flags & FMODE_WRITE))){
        pr_info("%s : Write permissions granted for minor number: %d\n", __func__, minor_n);
        return 0;
    }
    pr_info("%s : Permission denied for minor number: %d\n", __func__, minor_n);
    return -EPERM; // Return error code for permission denied
}

int module001_open(struct inode *inode, struct file *flip){
    pr_info("%s : Device opened.\n", __func__);

    int return_value;

    struct module_private_data *module_data;

    // 1. Find the device number file attempting to open by the user
    int minor_n = MINOR(inode->i_rdev);
    pr_info("%s : Opening device with minor number: %d\n", __func__, minor_n);

    // 2. Get the device structure from the inode
    module_data = container_of(inode->i_cdev, struct module_private_data, cdev);
    flip->private_data = module_data; // Store the private data in the file structure

    // 3. Check permissions
    return_value = Check_Permissions(module_data->perm, minor_n, flip->f_mode);
    if(!return_value) {
        pr_info("%s : Permission check passed for minor number: %d\n", __func__, minor_n);
    }
    else {
        pr_err("%s : Permission check failed for minor number: %d\n", __func__, minor_n);
    }
    return return_value; // Return 0 to indicate successful opening
}

int module001_release(struct inode *inode, struct file *flip){
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
    return_value = alloc_chrdev_region(&module_private_data.dev_num, 0, NO_OF_DEVICES, "devices");
    if(return_value < 0) {
        printk(KERN_ERR "Failed to allocate character device region.\n");
        goto out;
    }

    // Create a device class
    module_private_data.class_module = class_create("my_device_class");
    if(IS_ERR(module_private_data.class_module)) {
        printk(KERN_ERR "Failed to create device class.\n");
        return_value = PTR_ERR(module_private_data.class_module);
        goto unreg_chrdev;
    }

    for (int i = 0; i < NO_OF_DEVICES; i++) {
        pr_info("%s : Device created with major: %d, minor: %d\n", __func__,\
            MAJOR(module_private_data.dev_num + i), MINOR(module_private_data.dev_num + i));

        // 2. Initialize the cdev structure
        cdev_init(&module_private_data.module_data[i].cdev, &module001_fops);

        // 3. Add the cdev to the system with VFS
        module_private_data.module_data[i].cdev.owner = THIS_MODULE;
        return_value = cdev_add(&module_private_data.module_data[i].cdev, \
            module_private_data.dev_num + i, 1);
        if (return_value < 0) {
            printk(KERN_ERR "Failed to add cdev.\n");
            goto cdev_del;
        }

        // 4. Prepare device data
        module_private_data.device_module = device_create(
                                                        module_private_data.class_module, 
                                                        NULL,
                                                        module_private_data.dev_num + i, 
                                                        NULL,
                                                        "my_device-%d", i + 1);
        if(IS_ERR(module_private_data.device_module)) {
            printk(KERN_ERR "Failed to create device.\n");
            return_value = PTR_ERR(module_private_data.device_module);
            goto class_destroy;
        }
    }

    pr_info("Module initialized with device: %s\n", "my_device");
    return 0; // Return 0 to indicate successful loading

cdev_del:
    for (int i = 0; i < NO_OF_DEVICES; i++) {
        device_destroy(module_private_data.class_module, module_private_data.dev_num + i);
        cdev_del(&module_private_data.module_data[i].cdev);
    }
class_destroy:
    class_destroy(module_private_data.class_module);
unreg_chrdev:
    printk(KERN_ERR "Failed to add cdev.\n");
    unregister_chrdev_region(module_private_data.dev_num, NO_OF_DEVICES);
out: 
    printk(KERN_ERR "Module initialization failed.\n");
    return return_value; // Return error code

}

static void __exit __my_module_driver_exit(void){
    // Decontruct the device and free resources
    for (int i = 0; i < NO_OF_DEVICES; i++) {
        device_destroy(module_private_data.class_module, module_private_data.dev_num + i);
        cdev_del(&module_private_data.module_data[i].cdev);
    }
    class_destroy(module_private_data.class_module);
    unregister_chrdev_region(module_private_data.dev_num, NO_OF_DEVICES);

    pr_info("%s : Module exited and resources freed.\n", __func__);
    printk(KERN_INFO "My module driver is being unloaded.\n");
}



module_init(__my_module_driver_init);
module_exit(__my_module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("002");
MODULE_DESCRIPTION("A simple module driver for a pseudo character device");
MODULE_INFO(boarder, "Module001");
