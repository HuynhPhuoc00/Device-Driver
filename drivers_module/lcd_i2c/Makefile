# Module Kernel
obj-m += lcd_sysfs.o
lcd_sysfs-objs := lcd-sysfs.o lcd_i2c_drv.o

ARCH            = arm
CROSS_COMPILE   = arm-linux-gnueabihf-
KERNELDIR       = ../../source/linux_bbb_5.10/
HOST_KERNELDIR  = /lib/modules/$(shell uname -r)/build/

PATH_DTBS       = ../../source/linux_bbb_5.10/arch/arm/boot/dts/am335x-boneblack.dtb
DEBIAN_FOLDER   = /home/debian/drivers

# Build kernel module target
all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules

clean: clean-host app_clean
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) clean

help:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) help

# Build module host
host:
	$(MAKE) -C $(HOST_KERNELDIR) M=$(PWD) modules

clean-host:
	$(MAKE) -C $(HOST_KERNELDIR) M=$(PWD) clean

# Copy to board
copy-dtb:
	scp $(PATH_DTBS) debian@192.168.7.2:$(DEBIAN_FOLDER)

copy-drv:
	scp *.ko *.elf debian@192.168.7.2:$(DEBIAN_FOLDER)

# -----------------------------
# User-space Application
# -----------------------------
APP_NAME = lcd_app.elf
APP_OBJS = lcd_app.o

app: $(APP_OBJS)
	$(CROSS_COMPILE)gcc -static -o $(APP_NAME) $(APP_OBJS)

lcd_app.o: lcd_app.c
	$(CROSS_COMPILE)gcc -c -o $@ $<

# lcd_i2c_drv.o: lcd_i2c_drv.c
# 	$(CROSS_COMPILE)gcc -c -o $@ $<

app_clean:
	rm -f $(APP_OBJS) $(APP_NAME)
