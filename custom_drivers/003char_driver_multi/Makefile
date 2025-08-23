obj-m := module_n.o

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
KERNELDIR = ../../source/linux/
HOST_KERNELDIR = /lib/modules/$(shell uname -r)/build/

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