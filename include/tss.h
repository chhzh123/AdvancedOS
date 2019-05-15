// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** tss.h ******/

#ifndef tss_H
#define tss_H

#include "stdio.h"
#include "string.h"
#include <stdint.h>

struct tss_entry {
	uint32_t prevTss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
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

	// set stack and segments
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

void tss_set_stack (uint32_t kernelSS, uint32_t kernelESP) {

	// memset((void *)&tss, 0, sizeof(tss));
	tss.ss0 = kernelSS;
	tss.esp0 = kernelESP;
	// tss.iomap = sizeof(tss);
}

#endif // tss_H