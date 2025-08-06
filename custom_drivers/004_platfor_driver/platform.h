
#define RDONLY  0x01 // Read permissions
#define WRONLY  0x10 // Write permissions
#define RDWR    0x11 // Read Write permissions 

struct Module_platform_data{
    int size;
    int perm;
    const char *serial_number;
};

void module_platform_release(struct device *dev);