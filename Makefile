CC = gcc
CCFLAGS := -c -march=i386 -nostdlib -ffreestanding -Wall -Wextra
CCFLAGS += -Iinclude -lgcc -fno-PIE -nostartfiles
CCFLAGS += -m32 -masm=intel

AS = nasm
ASFLAGS = -felf32

LD = ld
LDFLAGS = -m elf_i386 -N --oformat binary

BUILD = build

IMG = mydisk.img
KERNEL = bootloader.bin 
DEBUG = 

all: build programs
	-rm -f $(IMG)
	/sbin/mkfs.msdos -C $(IMG) 1440
	dd if=$(BUILD)/bootloader.bin of=$(IMG) conv=notrunc

build:
	-mkdir $(BUILD)

programs: $(KERNEL) $(DEBUG)

bootloader.bin: bootloader.asm
	$(AS) -fbin $< -o $(BUILD)/$@

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