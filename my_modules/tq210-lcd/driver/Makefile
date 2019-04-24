obj-m := tq210_fb.o
KDIR := /home/myroot/TQ210N/linux-3.10.46
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
clean:
	rm -rf *.ko *.o *.mod.o *.mod.c *.symvers *.bak *.order .memdev.ko.cmd .memdev.mod.o.cmd .memdev.o.cmd .tmp_versions/ .tq210_fb.ko.cmd .tq210_fb.mod.o.cmd .tq210_fb.o.cmd 

