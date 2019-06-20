all:
	arm-linux-gcc -static -o syscall syscall.c
	arm-linux-gcc -static -o asm_test asm_test.c
	arm-linux-objdump -D syscall > syscall.dis
	arm-linux-objdump -D asm_test > asm_test.dis
	cp syscall asm_test ../../../rootfs

clean:
	rm syscall asm_test syscall.dis asm_test.dis