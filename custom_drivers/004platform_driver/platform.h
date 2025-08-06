
#define RDONLY  0x01 // Read permissions
#define WRONLY  0x10 // Write permissions
#define RDWR    0x11 // Read Write permissions 

#define MAX_DEVICES 10

struct Module_platform_data{
    int size;
    int perm;
    const char *serial_number;
};

/* Device private data*/
struct device_private_data{
    struct Module_platform_data pdata;
    char *buff;
    dev_t dev_num;
    struct cdev cdev;
};

/* Driver private data*/
struct driver_private_data{
    int total_devices;
    dev_t dev_num_base;
    struct class *class_module;
    struct device *device_module;
};

struct driver_private_data drv_data;

void module_platform_release(struct device *dev);