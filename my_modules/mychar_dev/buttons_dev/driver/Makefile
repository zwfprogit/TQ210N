obj-m := tq210_buttons_driver.o
KDIR := /home/myroot/TQ210/kernel/linux
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
clean:
	rm -rf *.ko *.order *.symvers .tmp_versions/ .tq210_buttons_driver.ko.cmd *.mod.c \
	*.mod.o *.mod.o.cmd *.o *.o.cmd 