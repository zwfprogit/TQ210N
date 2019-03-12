OBJ=bl2
LDFLAGS=-lgcc -L/usr/local/arm/4.3.2/lib/gcc/arm-none-linux-gnueabi/4.3.2/

$(OBJ).bin: start.o uart.o lcd.o main.o
	arm-linux-ld -Ttext 0x20000000 -o $(OBJ).elf $^ $(LDFLAGS)
	arm-linux-objcopy -O binary $(OBJ).elf $@
	arm-linux-objdump -D $(OBJ).elf > $(OBJ).dis
	
%.o : %.c
	arm-linux-gcc -c $< -o $@ -fno-builtin
%.o : %.S
	arm-linux-gcc -c $< -o $@
	
clean:
	rm *.o *.elf *.bin *.dis
