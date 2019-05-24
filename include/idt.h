// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** idt.h ******/

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// i86 defines 256 possible interrupt handlers (0-255)
#define MAX_INTERRUPTS		256
#define INTERRUPT_GATE      0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

// must be in the format 0D110, where D is descriptor type
#define IDT_DESC_BIT16		0x06	//00000110
#define IDT_DESC_BIT32		0x0E	//00001110
#define IDT_DESC_RING1		0x40	//01000000
#define IDT_DESC_RING2		0x20	//00100000
#define IDT_DESC_RING3		0x60	//01100000
#define IDT_DESC_PRESENT	0x80	//10000000

// interrupt descriptor
struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[MAX_INTERRUPTS];

// describes the structure for the processors idtr register
unsigned long idt_ptr[2];

void put_error(); // defined in "stdio.h"
// default handler to catch unhandled system interrupts.
void default_handler () {
	put_error("Error: Unhandled Exception!");
	for(;;);
}

// installs interrupt handler. When INT is fired, it will call this callback
int install_ir (uint32_t i, uint16_t type_attr, uint16_t selector, uint64_t irq) {

	if (i > MAX_INTERRUPTS)
		return 0;

	if (!irq)
		return 0;

	// get base address of interrupt handler
	uint64_t base = irq;

	// store base address into idt
	IDT[i].offset_lowerbits  = (uint16_t)(base & 0xffff);
	IDT[i].offset_higherbits = (uint16_t)((base >> 16) & 0xffff);
	IDT[i].zero              = 0;
	IDT[i].type_attr         = (uint8_t)(type_attr);
	IDT[i].selector          = selector;

	return	0;
}

// sets new interrupt vector
void setvect (int intno, uint64_t vect) {

	// install interrupt handler! This overwrites prev interrupt descriptor
	install_ir (intno, INTERRUPT_GATE, KERNEL_CODE_SEGMENT_OFFSET, vect);
}

void setvect_user (int intno, uint64_t vect) {
	install_ir (intno, INTERRUPT_GATE | IDT_DESC_RING3,
		KERNEL_CODE_SEGMENT_OFFSET, vect);
}

extern void load_idt(unsigned long *idt_ptr);

// initialize basic idt
int idt_init() {

	// null out the idt
	memset ((void*)&IDT[0], 0, sizeof(struct IDT_entry) * MAX_INTERRUPTS-1);

	// register default handlers
	for (int i = 0; i < MAX_INTERRUPTS; i++)
		setvect (i, (unsigned long)default_handler);

	// fill the IDT register
	unsigned long idt_address = (unsigned long)IDT;
	idt_ptr[0] = (sizeof(struct IDT_entry) * MAX_INTERRUPTS) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;

	load_idt(idt_ptr); // assembly

	return 0;
}

extern void keyboard_handler(void); // assembly
void kb_init(void)
{
	/* populate IDT entry of keyboard's interrupt */
	setvect(0x21,(unsigned long)keyboard_handler); // keyboard uses 33 interrupt
}

#endif // IDT_H