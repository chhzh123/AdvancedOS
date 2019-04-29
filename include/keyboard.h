// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** keyboard.h ******/
// https://github.com/arjun024/mkeykernel/blob/master/kernel.c

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#include "io.h"
// unsigned char port_byte_in(unsigned short port)
// void port_byte_out(unsigned short port, unsigned char data)
#include "scancode.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define MAX_BUF_LEN 1000
#define INVALID_KB_CHAR 0

extern void load_idt(unsigned long *idt_ptr);
extern void keyboard_handler(void);

static char kb_char;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void print_char(const char c);

void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/* Ports
	*         PIC1  PIC2
	* Command 0x20	0xA0
	* Data	  0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	port_byte_out(0x20, 0x11);
	port_byte_out(0xA0, 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	port_byte_out(0x21, 0x20);
	port_byte_out(0xA1, 0x28);

	/* ICW3 - setup cascading */
	port_byte_out(0x21, 0x00);
	port_byte_out(0xA1, 0x00);

	/* ICW4 - environment info */
	port_byte_out(0x21, 0x01);
	port_byte_out(0xA1, 0x01);
	/* Initialization finished */

	/* mask interrupts */
	port_byte_out(0x21, 0xff);
	port_byte_out(0xA1, 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr); // assembly
}

void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	port_byte_out(0x20, 0x20);

	status = port_byte_in(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = port_byte_in(KEYBOARD_DATA_PORT);
		if (keycode < 0)
			return;

		char ascii = asccode[(unsigned char) keycode][0];
		kb_char = ascii;
	}
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	port_byte_out(0x21, 0xFD);
}

void kb_close(void)
{
	port_byte_out(0x21, 0xFF);
}

char getchar()
{
	char c = INVALID_KB_CHAR;
	kb_char = INVALID_KB_CHAR;
	while (c == INVALID_KB_CHAR)
		c = kb_char;
	kb_char = INVALID_KB_CHAR;
	return c;
}

#endif // KEYBOARD_H