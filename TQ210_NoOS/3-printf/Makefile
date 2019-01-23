uart_addhead.bin:uart.bin
	../../Tools/checksum/s5pv210addheader $^ $@

uart.bin: start.o uart.o main.o
	arm-linux-ld -Ttext 0xD0020010 -o uart.elf $^
	arm-linux-objcopy -O binary -S uart.elf $@
	arm-linux-objdump -D uart.elf > uart.dis
	
%.o : %.c
	arm-linux-gcc -c $< -o $@ -fno-builtin 
%.o : %.S
	arm-linux-gcc -c $< -o $@
	
clean:
	rm *.o *.elf *.bin *.dis
