# ============================================================================
# TARGET
# ============================================================================

# Project taget
obj-m += i2c_driver.o
i2c_driver-objs := i2c_lcd_sysfs.o lcd_i2c_drv.o

# ============================================================================
# CONFIGURATION VARIABLES
# ============================================================================

# Project structure
PROJ_DIR 		:= ../../../

# Cross compilation settings
ARCH            := arm
CROSS_COMPILE   := arm-linux-gnueabihf-

# Kernel directories
KERNELDIR       := $(PROJ_DIR)/source/linux/
HOST_KERNELDIR  := /lib/modules/$(shell uname -r)/build/

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