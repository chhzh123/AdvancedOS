// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** tss.h ******/

#ifndef TSS_H
#define TSS_H

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

struct tss_entry TSS;

void tss_install () {
	__asm__ volatile (
			"ltr ax\n\t"
			:
			:"a"(5 << 3)
			:
			);
}

// extern void load_tss(unsigned long* tss_ptr); // assembly

void tss_set_stack (uint16_t kernelSS, uint16_t kernelESP) {

	memset((void *)&TSS, 0, sizeof(TSS));
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.iomap = sizeof(TSS);
}

// void install_tss (uint32_t idx, uint16_t kernelSS, uint16_t kernelESP) {

// 	// install TSS descriptor
// 	uint32_t base = (uint32_t) &TSS;

// 	// initialize TSS
// 	memset ((void*) &TSS, 0, sizeof (struct tss_entry));

// 	// set stack and segments
// 	TSS.ss0 = kernelSS;
// 	TSS.esp0 = kernelESP;
// 	TSS.cs = 0x0b;
// 	TSS.ss = 0x13;
// 	TSS.es = 0x13;
// 	TSS.ds = 0x13;
// 	TSS.fs = 0x13;
// 	TSS.gs = 0x13;

// 	put_info("TSS: Initialized TSS Done!");

// 	// install descriptor
// 	// Warning: TSS MUST be put in GDT, not in LDT or IDT
// 	gdt_set_descriptor (idx, base, base + sizeof (struct tss_entry),
// 		GDT_DESC_ACCESS|GDT_DESC_EXEC_CODE|GDT_DESC_DPL|GDT_DESC_MEMORY,
// 		0);
// 	put_info("TSS: Set GDT Done!");

// 	// reload all segment registers
// 	load_gdt((uint32_t*)&_gdtr); // assembly
// 	put_info("TSS: Reload GDT!");

// 	// flush tss
// 	load_tss(idx << 3);
// 	put_info("TSS: Load TSS Done!");
// }

// void tss_init()
// {
// 	install_tss(5,0x10,0); // the 5th item in gdt (0x2b)
// }

#endif // TSS_H