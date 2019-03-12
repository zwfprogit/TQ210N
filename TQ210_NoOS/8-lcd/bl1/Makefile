OBJ=bl1
LDFLAGS=-lgcc -L/usr/local/arm/4.3.2/lib/gcc/arm-none-linux-gnueabi/4.3.2/
$(OBJ)_addhead.bin:$(OBJ).bin
	../../../Tools/checksum/s5pv210addheader $^ $@
$(OBJ).bin: start.o clock.o uart.o ddr.o copy.o  main.o
	arm-linux-ld -Ttext 0xD0020010 -o $(OBJ).elf $^ $(LDFLAGS)
	arm-linux-objcopy -O binary $(OBJ).elf $@
	arm-linux-objdump -D $(OBJ).elf > $(OBJ).dis
	
%.o : %.c
	arm-linux-gcc -c $< -o $@ -fno-builtin
%.o : %.S
	arm-linux-gcc -c $< -o $@
	
clean:
	rm *.o *.elf *.bin *.dis
