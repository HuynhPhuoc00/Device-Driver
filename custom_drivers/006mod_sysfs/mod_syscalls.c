#include"module_driver_dt_sysfs.h"

loff_t module_lseek(struct file *flip, loff_t off, int whence){
    return 0;
}

ssize_t module_read(struct file *flip, char __user *buff, size_t count, loff_t *f_pos){
    return 0;
}

ssize_t module_write(struct file *flip, const char __user *buff, size_t count, loff_t *f_pos){
    return -ENOMEM; 
}

int Check_Permissions(int perm, int minor_n, int flags){
    pr_info("Checking permissions for minor number: %d\n", minor_n);
    
    if(perm == RDWR){
        return 0;
    }

   // Read permissions
    if((perm == RDONLY) && ((flags & FMODE_READ) && !(flags & FMODE_WRITE))){
        pr_info("Read permissions granted for minor number: %d\n", minor_n);
        return 0;
    }

    // Write permissions
    if((perm == WRONLY) && (!(flags & FMODE_READ) && (flags & FMODE_WRITE))){
        pr_info("Write permissions granted for minor number: %d\n", minor_n);
        return 0;
    }
    pr_info("Permission denied for minor number: %d\n", minor_n);
    return -EPERM; // Return error code for permission denied
}

int module_open(struct inode *inode, struct file *flip){
    return 0;
}

int module_release(struct inode *inode, struct file *flip){
    pr_info("Device closed.\n");
    return 0; // Return 0 to indicate successful closing
}