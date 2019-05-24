CC = gcc
CCFLAGS := -c -march=i386 -nostdlib -ffreestanding -Wall
CCFLAGS += -Iinclude -lgcc -fno-PIE -nostartfiles
CCFLAGS += -m32 -masm=intel

AS = nasm
ASFLAGS = -felf32

LD = ld
LDFLAGS = -m elf_i386 -N --oformat binary

BOCHS = bochs
BOCHSFLAGS = -q -f bochsrc.bxrc

BUILD = build

IMG = bootflpy.img
KERNEL = bootloader.bin kernel.bin

HARDDISK = mydisk.hdd
USRDIR = usr
USR = prg1.com prg2.com prg3.com prg4.com box.com sys_test.com fork_test.out

ifdef DEBUG
CCFLAGS += -DDEBUG
DEBUGFILES = kernel.s
endif

all: build programs
	-rm -f $(IMG)
	/sbin/mkfs.msdos -C $(IMG) 1440
	dd if=$(BUILD)/bootloader.bin of=$(IMG) conv=notrunc
	dd if=$(BUILD)/kernel.bin of=$(IMG) seek=1 conv=notrunc # 2nd sector
	/sbin/mkfs.msdos -C $(HARDDISK) 1440
	dd if=$(BUILD)/$(USRDIR)/prg1.com of=$(HARDDISK) conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/prg2.com of=$(HARDDISK) seek=2 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/prg3.com of=$(HARDDISK) seek=4 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/prg4.com of=$(HARDDISK) seek=6 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/box.com of=$(HARDDISK) seek=8 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/sys_test.com of=$(HARDDISK) seek=10 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/fork_test.out of=$(HARDDISK) seek=12 conv=notrunc

build:
	-mkdir $(BUILD)
	-mkdir $(BUILD)/$(USRDIR)

programs: $(KERNEL) $(foreach prg,$(USR),$(USRDIR)/$(prg)) $(DEBUGFILES)

bootloader.bin: bootloader.asm
	$(AS) -fbin $< -o $(BUILD)/$@
kernel.bin: kernel_entry.o kernel.o
	$(LD) $(LDFLAGS) -Ttext 0x7e00 $(BUILD)/kernel_entry.o $(BUILD)/kernel.o -o $(BUILD)/$@

%.o : %.asm
	$(AS) $(ASFLAGS) $< -o $(BUILD)/$@
%.o : %.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/$@
%.com : %.asm
	$(AS) $< -o $(BUILD)/$@

# generate elf file
usr/fork_test.out: usr/fork_test.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fork_test.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fork_test.o -o $(BUILD)/$@

# debug
%.s : %.c
	$(CC) $(CCFLAGS) -S $< -o $@

# readelf -h fork_test.out ## show hex
# readelf -S fork_test.out ## show section headers
# readelf -l fork_test.out ## show program headers
# objdump -d fork_test.out
# hd fork_test.out

# emulation
run: bochs

bochs:
	$(BOCHS) $(BOCHSFLAGS)

.PHONY : clean
clean :
	-rm -rf $(BUILD)
	-rm -f *.o *.bin *.com *.elf *.s $(IMG) $(HARDDISK) $(PROGS) *.lock bochsout.txt