obj-m := mod_sysfs.o
mod_sysfs-objs += module_driver_dt_sysfs.o mod_syscalls.o

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
KERNELDIR = ../../source/linux/
HOST_KERNELDIR = /lib/modules/$(shell uname -r)/build/

PATH_DTBS = ../../source/linux/arch/arm/boot/dts/am335x-boneblack.dtb
DEBIAN_FOLDER = /home/debian/drivers

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) help
host:
	make -C $(HOST_KERNELDIR) M=$(PWD) modules
clean-host:
	make -C $(HOST_KERNELDIR) M=$(PWD) clean
copy-dtb:
	scp  $(PATH_DTBS) debian@192.168.7.2:$(DEBIAN_FOLDER)
copy-drv:
	scp *.ko debian@192.168.7.2:$(DEBIAN_FOLDER)