# Advanced Operating System (Protected Mode)

This is an advanced operating system running in **protected mode** designed for *Operating Systems (Laboratory)* - Spring 2019 @ SYSU. The lecturer of this course is Yingbiao Ling.


## Environment

The environment setting of my computer is listed below.
* Windows 10 + Ubuntu 18.04 (LTS) subsystem
* gcc 7.3.0 + nasm 2.13.02 + GNU ld (Binutils) 2.3.0
* GNU Make 4.1
* Oracle VM VirtualBox 6.0.8
* Bochs 2.6.9

The virtual machine of VirtualBox has 4M memory and uses a 1.44M virtual floppy disk `bootflpy.img` to bootstrap. Also, a 1.44M virtual hard-disk `mydisk.hdd` is mounted to the system.


## Compilation

Please follow the commands below to compile the OS:

```bash
$ git clone https://github.com/chhzh123/AdvancedOS.git
$ cd AdvancedOS
$ make

# compile the user programs
$ make programs

# write into virtual disk
$ make disk
```

You need not have the same environment with my computer, but some basic tools, like C/C++ compiler, x86-assembler, and virtual machine platforms, need to be in work. The tools can be configured for `Makefile` environment variables.

However, using different tools may lead to totally different results, please be careful.

The first `make` command will generate the floppy disk `bootflpy.img`, which contains the bootstrap program and the main OS kernel.

The virtual disk in FAT12 format storing user programs has been provided in the repository. If you do not want to recompile the user programs, you can directly use `mydisk.hdd`.


## Configuration

In VirtualBox, you need to configure the system after you create a virtual machine instance.

In `Settings-General`,
* `Type` and `Version` are `Other-Other/Unknown`

In `Settings-System`,
* `Base Memory` is `4MB`
* Change `Boot Order` to `Floppy` first

In `Settings-Storage`,
* Add a new `Controller: Floppy`, with type `I82078`, and attach the `bootflpy.img` as `Floppy Device 0`
* Add a new `Controller: IDE`, with type `PIIX4`, and attach the `mydisk.hdd` as `IDE Primary Master`


## File Organization

* The main function of the kernel can be found in `kernel_entry.asm` and `kernel.c`.
* The C header files (kernel facilities) are in `/include`.
* User programs are in `/usr`.


## Current Functionality

The operating system named "CHZOS" enables **32-bit protected mode**.

* Protected mode support
	- Global Descriptor Table (GDT)
	- Interrupt Descriptor Table (IDT): including exception handlers
	- Task State Segment (TSS)
	- Switch between kernel mode (ring 0) and user mode (ring 3)
* Hardware Abstraction Layer (HAL)
	- Integrated Drive Electronics (IDE): disk loader
	- Programmable Interrupt Controller (PIC)
	- Programmable Interval Timer (PIT)
	- Keyboard driver
* Shell
	- Basic I/O interface: use video RAM to show
	- Simple parser for the input commands
* Multiple processes management
	- Five-state process model
	- Parent and child processes management with `fork`, `wait`, `exit`, etc.
	- Semaphore support (P/V operation)
	- `pthread`-like multi-threaded support
	- Time-sharing, multi-tasking
	- Currently use round-robin scheduling
* File System
	- FAT12 format
	- Support basic file operations, such as `cd`, `ls`, `rm`, `cp`, etc.
	- Support C file operations, such as `fopen`, `fclose`, `fgets`, `fputs`, etc.
* User programs
	- ELF file support
	- Standard libraries support (I/O, string, etc.)
	- Load, schedule, and execute
	- System call support (`0x80` - `0x82`)
* Multiple consoles support
	- Currently only support static operations for multiple consoles


## Tutorials

I wrote a tutorial on protected mode operating system in Chinese, please refer to [my blog](https://chhzh123.github.io/summary/os-dev/) (not done yet!).

For experimental reports, please refer to my [repository](https://github.com/chhzh123/Assignments/tree/master/OperatingSystems).


## Reference
1. OS Development Series, <http://www.brokenthorn.com/Resources/OSDevIndex.html>
2. Roll your own toy UNIX-clone OS, <http://www.jamesmolloy.co.uk/tutorial_html/>
3. The little book about OS development, <http://littleosbook.github.io/>
4. Writing a Simple Operating System from Scratch, <http://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf>
5. Intel&reg; 64 and IA-32 Architectures Software Developer Manuals, <https://software.intel.com/en-us/articles/intel-sdm>
6. UCore OS Lab, <https://github.com/chyyuu/ucore_os_lab>
7. CMU CS 15-410, Operating System Design and Implementation, <https://www.cs.cmu.edu/~410/>

## TO DO
* [] Memory management, including paging, `malloc`, etc.
* [] Better encapsulation of the user APIs, such as `printf` and `scanf`