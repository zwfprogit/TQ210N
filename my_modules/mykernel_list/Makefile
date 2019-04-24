obj-m := mylist_test.o

KDIR := ../../kernel/linux

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
	
clean:
	rm -rf *.o *.ko *.order *.symvers .* *.mod.c
