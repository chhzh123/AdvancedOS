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
USR = prg1.com prg2.com prg3.com prg4.com box.com syscall.com
USR += fork.out fork2.out bank.out fruit.out pro-con.out hello.out matmal.out file.out

ifdef DEBUG
CCFLAGS += -DDEBUG
DEBUGFILES = kernel.s
endif

all: build osker
	-rm -f $(IMG)
	/sbin/mkfs.msdos -C $(IMG) 1440
	dd if=$(BUILD)/bootloader.bin of=$(IMG) conv=notrunc
	dd if=$(BUILD)/kernel.bin of=$(IMG) seek=1 conv=notrunc # 2nd sector

build:
	-mkdir $(BUILD)

osker: $(KERNEL)

bootloader.bin: bootloader.asm
	$(AS) -fbin $< -o $(BUILD)/$@

kernel.bin: kernel_entry.o kernel.o
	$(LD) $(LDFLAGS) -Ttext 0x7e00 $(BUILD)/kernel_entry.o $(BUILD)/kernel.o -o $(BUILD)/$@

kernel_entry.o : kernel_entry.asm
	$(AS) $(ASFLAGS) $< -o $(BUILD)/$@

kernel.o : kernel.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/$@

# user programs
prg:
	-rm $(HARDDISK)
	/sbin/mkfs.msdos -C $(HARDDISK) 1440
	-mkdir disk
	mount $(HARDDISK) disk
	mkdir disk/myfolder
	mkdir disk/myfolder/folder
	mkdir disk/testfold
	cp $(BUILD)/usr/*.com disk 
	cp $(BUILD)/usr/*.out disk
	cp usr/test.txt disk/myfolder/folder/test.txt
	umount disk
	-rm -rf disk

programs: buildprg $(foreach prg,$(USR),$(USRDIR)/$(prg)) $(DEBUGFILES)

buildprg:
	-mkdir $(BUILD)
	-mkdir $(BUILD)/$(USRDIR)
%.com : %.asm
	$(AS) $< -o $(BUILD)/$@

# generate elf file
# %.out : %.c
# 	$(CC) $(CCFLAGS) $< -o $(BUILD)/$@
# 	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/$@.o -o $(BUILD)/$@
usr/fork.out: usr/fork.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fork.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fork.o -o $(BUILD)/$@
usr/fork2.out: usr/fork2.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fork2.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fork2.o -o $(BUILD)/$@
usr/bank.out: usr/bank.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/bank.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/bank.o -o $(BUILD)/$@
usr/fruit.out: usr/fruit.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/fruit.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/fruit.o -o $(BUILD)/$@
usr/pro-con.out: usr/pro-con.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/pro-con.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/pro-con.o -o $(BUILD)/$@
usr/hello.out: usr/hello.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/hello.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/hello.o -o $(BUILD)/$@
usr/matmal.out: usr/matmal.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/matmal.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/matmal.o -o $(BUILD)/$@
usr/file.out: usr/file.c
	$(CC) $(CCFLAGS) $< -o $(BUILD)/file.o
	$(LD) -m elf_i386 -Tusr/link.ld $(BUILD)/file.o -o $(BUILD)/$@

# debug
%.s : %.c
	$(CC) $(CCFLAGS) -S $< -o $@

# readelf -h fork.out ## show hex
# readelf -S fork.out ## show section headers
# readelf -l fork.out ## show program headers
# objdump -d fork.out ## show assembly
# hd fork.out

# emulation
run: bochs

bochs:
	$(BOCHS) $(BOCHSFLAGS)

.PHONY : clean
clean :
	-rm -rf $(BUILD)
	-rm -f *.o *.bin *.com *.elf *.s $(IMG) $(PROGS) *.lock bochsout.txt

cleandisk:
	-rm $(HARDDISK)