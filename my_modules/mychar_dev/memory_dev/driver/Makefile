obj-m := memdev.o
KDIR := /home/myroot/TQ210/kernel/linux
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
clean:
	rm -rf *.ko *.o *.mod.o *.mod.c *.symvers *.bak *.order .memdev.ko.cmd .memdev.mod.o.cmd .memdev.o.cmd .tmp_versions/

