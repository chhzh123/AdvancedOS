CC = gcc
CCFLAGS := -c -march=i386 -nostdlib -ffreestanding -Wall -Wextra
CCFLAGS += -Iinclude -lgcc -fno-PIE -nostartfiles
CCFLAGS += -m32

AS = nasm
ASFLAGS = -felf32

LD = ld
LDFLAGS = -m elf_i386 -N --oformat binary

BUILD = build

IMG = mydisk.img
KERNEL = bootloader.bin kernel.bin
DEBUG = kernel.s

all: build programs
	-rm -f $(IMG)
	/sbin/mkfs.msdos -C $(IMG) 1440
	dd if=$(BUILD)/bootloader.bin of=$(IMG) conv=notrunc
	dd if=$(BUILD)/kernel.bin of=$(IMG) seek=1 conv=notrunc # 2nd sector

build:
	-mkdir $(BUILD)

programs: $(KERNEL) $(DEBUG)

bootloader.bin: bootloader.asm
	$(AS) -fbin $< -o $(BUILD)/$@
kernel.bin: kernel_entry.o kernel.o
	$(LD) $(LDFLAGS) -Ttext 0x1000 $(BUILD)/kernel_entry.o $(BUILD)/kernel.o -o $(BUILD)/$@

%.o : %.asm
	$(AS) $(ASFLAGS) $< -o $(BUILD)/$@
%.o : %.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/$@

# debug
%.s : %.c
	$(CC) $(CCFLAGS) -S $< -o $@

.PHONY : clean
clean :
	-rm -rf $(BUILD)
	-rm -f *.o *.bin *.com *.elf *.s $(IMG) $(PROGS)