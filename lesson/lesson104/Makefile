CC = riscv64-linux-gnu-gcc
LD = riscv64-linux-gnu-ld
OBJCOPY = riscv64-linux-gnu-objcopy
CFLAGS = -march=rv64g -mabi=lp64 -ffreestanding -nostdlib -mcmodel=medany -fno-pie -fno-pic -g -MMD -MP
LDFLAGS = -T linker.ld
OBJS :=riscv.o main.o uart.o plic.o trap.o console.o string.o shell.o mmu.o page.o heap.o tests.o kernel.o task.o
DEPS := $(OBJS:.o=.d)
kernel.elf : $(OBJS) 
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
-include $(DEPS)
clean:
	rm -f *.o *.d kernel.elf kernel.bin

