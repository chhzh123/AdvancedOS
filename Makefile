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
USR = prg1.com prg2.com prg3.com prg4.com box.com sys_test.com
USR += fork_test.out fork2.out bank.out fruit.out prod_cons.out hello_world_thread.out

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
	dd if=$(BUILD)/$(USRDIR)/fork2.out of=$(HARDDISK) seek=42 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/bank.out of=$(HARDDISK) seek=72 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/fruit.out of=$(HARDDISK) seek=102 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/prod_cons.out of=$(HARDDISK) seek=132 conv=notrunc
	dd if=$(BUILD)/$(USRDIR)/hello_world_thread.out of=$(HARDDISK) seek=162 conv=notrunc

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
# %.out : %.c
# 	$(CC) $(CCFLAGS) $< -o $(BUILD)/$@
# 	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/$@.o -o $(BUILD)/$@
usr/fork_test.out: usr/fork_test.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fork_test.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fork_test.o -o $(BUILD)/$@
usr/fork2.out: usr/fork2.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fork2.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fork2.o -o $(BUILD)/$@
usr/bank.out: usr/bank.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/bank.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/bank.o -o $(BUILD)/$@
usr/fruit.out: usr/fruit.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fruit.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fruit.o -o $(BUILD)/$@
usr/prod_cons.out: usr/prod_cons.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/prod_cons.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/prod_cons.o -o $(BUILD)/$@
usr/hello_world_thread.out: usr/hello_world_thread.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/hello_world_thread.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/hello_world_thread.o -o $(BUILD)/$@

# debug
%.s : %.c
	$(CC) $(CCFLAGS) -S $< -o $@

# readelf -h fork_test.out ## show hex
# readelf -S fork_test.out ## show section headers
# readelf -l fork_test.out ## show program headers
# objdump -d fork_test.out ## show assembly
# hd fork_test.out

# emulation
run: bochs

bochs:
	$(BOCHS) $(BOCHSFLAGS)

.PHONY : clean
clean :
	-rm -rf $(BUILD)
	-rm -f *.o *.bin *.com *.elf *.s $(IMG) $(HARDDISK) $(PROGS) *.lock bochsout.txt