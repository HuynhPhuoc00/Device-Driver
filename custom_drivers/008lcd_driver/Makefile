obj-m := gpio-sysfs.o

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
KERNELDIR = ../../source/linux_bbb_5.10/
HOST_KERNELDIR = /lib/modules/$(shell uname -r)/build/

PATH_DTBS = ../../source/linux_bbb_5.10/arch/arm/boot/dts/am335x-boneblack.dtb
DEBIAN_FOLDER = /home/debian/drivers

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules

clean: clean-host
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
	scp *.ko *.elf debian@192.168.7.2:$(DEBIAN_FOLDER)

app: lcd_app.o lcd.o gpio.o lcd_app.elf

lcd_app.o: lcd_app.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

lcd.o: lcd.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

gpio.o: gpio.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

lcd_app.elf: lcd_app.o lcd.o gpio.o
	$(CROSS_COMPILE)gcc -static -o $@ $^