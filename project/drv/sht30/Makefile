# ============================================================================
# TARGET
# ============================================================================

# Project taget
obj-m += sht_driver.o
sht_driver-objs := sht3x_sysfs.o sht3x.o

# ============================================================================
# CONFIGURATION VARIABLES
# ============================================================================

# Project structure
PROJ_DIR 		:= ../../..

# Cross compilation settings
ARCH            := arm
CROSS_COMPILE   := arm-linux-gnueabihf-

# Kernel directories
KERNELDIR       := $(PROJ_DIR)/source/linux/
HOST_KERNELDIR  := /lib/modules/$(shell uname -r)/build/
DEBIAN_FOLDER = /home/debian/drivers/
all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(CURDIR) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(CURDIR) help
host:
	make -C $(HOST_KERNELDIR) M=$(CURDIR) modules
clean-host:
	make -C $(HOST_KERNELDIR) M=$(CURDIR) clean

copy-drv:
	scp *.ko debian@192.168.7.2:$(DEBIAN_FOLDER)