obj-m := gpio-sysfs.o

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
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) clean
	rm *.elf
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) help

host:
	make -C $(HOST_KERNELDIR) M=$(PWD) modules

clean-host:
	make -C $(HOST_KERNELDIR) M=$(PWD) clean

elf: app.o 74hc.o gpio.o app.elf

app.o: app.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

74hc.o: 74hc.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

gpio.o: gpio.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

app.elf: app.o 74hc.o gpio.o
	$(CROSS_COMPILE)gcc -static -o $@ $^