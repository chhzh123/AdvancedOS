// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** keyboard.h ******/
// https://github.com/arjun024/mkeykernel/blob/master/kernel.c

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#include "io.h"
#include "scancode.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define INVALID_KB_CHAR 0

static char kb_char;
static bool __ctrl = false;

void change_terminal();

void keyboard_handler_main(void)
{
	unsigned char status;
	unsigned char scancode;

	/* write EOI */
	port_byte_out(0x20, 0x20);

	status = port_byte_in(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		scancode = port_byte_in(KEYBOARD_DATA_PORT);
		if (scancode < 0)
			return;
		if (scancode & 0x80) {
			if (scancode - 0x80 == KEY_CTRL)
				__ctrl = false;
		} else {
			if (scancode == KEY_CTRL)
				__ctrl = true;
			else if (__ctrl && scancode == KEY_F1)
				change_terminal(0);
			else if (__ctrl && scancode == KEY_F2)
				change_terminal(1);
			else if (__ctrl && scancode == KEY_F3)
				change_terminal(2);
			else {
				char ascii = asccode[(unsigned char) scancode][0];
				kb_char = ascii;
			}
		}
	}
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