// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** tss.h ******/

#ifndef tss_H
#define tss_H

#include "stdio.h"
#include "string.h"
#include <stdint.h>

#define KERNEL_STACK_SIZE 2048		 // Use a 2kb kernel stack

/*
 * When operating in protected mode, a TSS and TSS descriptor 
 * MUST be created for at least one task, and the
 * segment selector for the TSS must be loaded into
 * the task register (using the LTR instruction).
 */
struct tss_entry {
	uint32_t prevTss;
	uint32_t esp0;
	uint32_t ss0; // ring 0
	uint32_t esp1;
	uint32_t ss1; // ring 1
	uint32_t esp2;
	uint32_t ss2; // ring 2
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap;
} __attribute__((packed));

struct tss_entry tss;

void tss_install (uint32_t sel) {

	memset((void *)&tss, 0, sizeof(tss));

	// Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These specify what
	// segments should be loaded when the processor switches to kernel mode. Therefore
	// they are just our normal kernel code/data segments - 0x08 and 0x10 respectively,
	// but with the last two bits set, making 0x0b and 0x13. The setting of these bits
	// sets the RPL (requested privilege level) to 3, meaning that this TSS can be used
	// to switch to kernel mode from ring 3.
	tss.ss0 = KERNEL_DS;
	tss.esp0 = 0x0;
	tss.cs = KERNEL_CS | DPL_USER;
	tss.ss = tss.es = tss.ds = tss.fs = tss.gs = KERNEL_DS | DPL_USER;

	__asm__ volatile (
			"ltr ax\n\t"
			:
			:"a"(sel << 3)
			:
			);
}

void tss_set_stack (uint32_t ss, uint32_t esp) {

	// memset((void *)&tss, 0, sizeof(tss));
	tss.ss0 = ss;
	tss.esp0 = esp;
	// tss.ebp = ebp;
	// tss.iomap = sizeof(tss);
}

#endif // tss_H