CC = riscv64-linux-gnu-gcc
LD = riscv64-linux-gnu-ld
OBJCOPY = riscv64-linux-gnu-objcopy
CFLAGS = -march=rv64g -mabi=lp64 -ffreestanding -nostdlib
LDFLAGS = -T linker.ld
OBJS :=boot.o main.o
kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
kernel.bin : kernel.elf
	$(OBJCOPY) -O binary $< $@
%.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
all : kernel.bin
run : all
	qemu-system-riscv64 -machine virt -nographic -bios none  -kernel kernel.bin
debug : all
	qemu-system-riscv64 -machine virt -nographic -bios none  -kernel kernel.bin -s -S
clean:
	rm -f *.o kernel.elf kernel.bin

