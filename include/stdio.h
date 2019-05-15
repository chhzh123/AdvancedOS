// Copyright(c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

#ifndef STDIO_H
#define STDIO_H

#include "io.h"
#include "string.h"
#include <stdarg.h>

// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define MAX_BUF_LEN 1000

enum Color{
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GREY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10, //a
	LIGHT_CYAN = 11, // b
	LIGHT_RED = 12, // c
	LIGHT_MAGENTA = 13, // d
	LIGHT_BROWN = 14, // e
	WHITE = 15 // f
};

int ATTRIBUTE_VAL = 0x0007;

/* Copy bytes from one place to another */
void memory_copy(char* source, char* dest, int no_bytes) {
	for (int i = 0; i < no_bytes; i++) {
		*(dest + i) = *(source + i);
	}
}

int get_color(enum Color fg, enum Color bg)
{
	return (fg | (bg << 4));
}

void set_color(enum Color fg, enum Color bg)
{
	ATTRIBUTE_VAL = (fg | (bg << 4));
}

int get_screen_offset(int col,int row)
{
	return (row * MAX_COLS + col) * 2;
}

void set_cursor(int offset){
	offset /= 2; // Convert from cell offset to char offset
	// This is similar to get_cursor, only now we write
	// bytes to those internal device registers
	port_byte_out(REG_SCREEN_CTRL, 14);
	port_byte_out(REG_SCREEN_DATA, (unsigned char)((offset >> 8) & 0x00FF));
	port_byte_out(REG_SCREEN_CTRL, 15);
	port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0x00FF));
}

int get_cursor() {
	// The device uses its control register as an index
	// to select its internal registers, of which we are
	// interested in:
	// reg 14: which is the high byte of the cursor's offset
	// reg 15: which is the low byte of the cursor's offset
	// Once the internal register has been selected, we may read or
	// write a byte on the data register
	port_byte_out(REG_SCREEN_CTRL, 14);
	int offset = port_byte_in(REG_SCREEN_DATA) << 8;
	port_byte_out(REG_SCREEN_CTRL, 15);
	offset += port_byte_in(REG_SCREEN_DATA);
	// Since the cursor offset reported by the VGA hardware is the
	// number of characters, we multiply by two to convert it to
	// a character cell offset
	return offset * 2;
}

/* Advance the text cursor, scrolling the video buffer if necessary */
int handle_scrolling(int cursor_offset) {
	// If the cursor is within the screen, return it unmodified
	if(cursor_offset < MAX_ROWS * MAX_COLS *2) {
		return cursor_offset;
	}
	/* Shuffle the rows back one */
	int i;
	for(i = 1; i < MAX_ROWS; i ++) {
		memory_copy((char*)(get_screen_offset(0, i) + VIDEO_ADDRESS),
					(char*)(get_screen_offset(0, i-1) + VIDEO_ADDRESS), MAX_COLS * 2);
	}
	/* Blank the last line by setting all bytes to 0 */
	char* last_line = (char*)(get_screen_offset(0, MAX_ROWS -1) + VIDEO_ADDRESS);
	for(i = 0; i < MAX_COLS *2; i ++) {
		last_line[i] = 0;
	}
	// Move the offset back one row, such that it is now on the last
	// row, rather than off the edge of the screen
	cursor_offset -= 2 * MAX_COLS;
	// Return the updated cursor position
	return cursor_offset;
}

/* Print a char on the screen at col, row, or at cursor position */
void putchar(char character, int col, int row, char attribute_byte) {
	/* Create a byte (char) pointer to the start of video memory */
	unsigned char* vidmem = (unsigned char*) VIDEO_ADDRESS;
	/* If attribute byte is zero, assume the default style */
	if (!attribute_byte) {
		attribute_byte = ATTRIBUTE_VAL;
	}
	/* Get the video memory offset for the screen location */
	int offset;
	/* If col and row are non-negative, use them for offset */
	if (col >= 0 && row >= 0) {
		offset = get_screen_offset(col, row);
	/* Otherwise, use the current cursor position */
	} else {
		offset = get_cursor();
	}
	// If we see a newline character, set offset to the end of
	// current row, so it will be advanced to the first col
	// of the next row.
	if (character == '\n') {
		int rows = offset / (2 * MAX_COLS);
		offset = get_screen_offset(79, rows);
	// Otherwise, write the character and its attribute byte to
	// video memory at our calculated offset
	} else if (character == '\b') {
		offset -= 2;
		vidmem[offset] = ' ';
		vidmem[offset+1] = attribute_byte;
		offset -= 2;
	} else {
		vidmem[offset] = character;
		vidmem[offset+1] = attribute_byte;
	}
	// Update the offset to the next character cell, which is
	// two bytes ahead of the current cell
	offset += 2;
	// Make scrolling adjustment, for when we reach the bottom
	// of the screen
	offset = handle_scrolling(offset);
	// Update the cursor position on the screen device
	set_cursor(offset);
}

void print_at(const char* message, int col, int row) {
	// Update the cursor if col and row not negative
	if(col >= 0 && row >= 0) {
		set_cursor(get_screen_offset(col, row));
	}
	// Loop through each char of the message and print it
	int i = 0;
	while(message[i] != 0) {
		putchar(message[i++], col, row, ATTRIBUTE_VAL);
	}
}

void print(const char* message) {
	print_at(message, -1, -1);
}

void print_char(const char c) {
	putchar(c, -1, -1, ATTRIBUTE_VAL);
}

void print_int(const int x, const int base) {
	char str[MAX_BUF_LEN];
	itoa(x,str,base);
	print(str);
}

void clear_screen() {
	int row = 0;
	int col = 0;
	/* Loop through video memory and write blank characters */
	for(row = 0; row < MAX_ROWS; row ++) {
		for(col = 0; col < MAX_COLS; col ++) {
			putchar(' ', col, row, ATTRIBUTE_VAL);
		}
	}
	// Move the cursor back to the top left
	set_cursor(get_screen_offset(0, 0));
}

char getchar(); // defined in keyboard.h

void getline(char* res)
{
	int i = 0;
	while(1){
		char ch = getchar();
		if (ch == '\b'){
			if (i == 0)
				continue;
			res[--i] = '\0';
			set_cursor(get_cursor()-2);
			print_char(' ');
			set_cursor(get_cursor()-2);
			continue;
		}
		res[i++] = ch;
		print_char(ch);
		if (ch == '\r' || ch == '\n'){
			res[--i] = '\0';
			break;
		}
	}
}

int read_int(const char* s, int* readNum) {
    int sign = 1;
    int cnt = 0;
    if (*s == '-') {
        sign = -1;
        s++;
        cnt++;
    }
    int ret = 0;
    while (*s && *s >= '0' && *s <= '9') {
        ret = ret * 10 + (*s - '0');
        s++;
        cnt++;
    }
    *readNum = (int) ret * sign;
    return cnt;
}

void printf(const char* format, ...){
	int narg = 0;
	int i = 0;
	int padding = 0;
	for (i = 0; format[i]; i++)
		if (format[i] == '%')
			narg++;

	va_list valist;
	va_start(valist, format);

	for (i = 0; format[i]; ++i) {
		int digitLength = 0;
		if (format[i] == '%') {
			if ((format[i+1] >= '0' && format[i+1] <= '9') || (format[i+1] == '-')) {
				digitLength = read_int(format + i + 1,&padding);
			}
			if (format[i + digitLength + 1] == 'd' || format[i + digitLength + 1] == 'i') {
				int data = va_arg(valist, int);
				print_int(data,10);
			} else if (format[i + digitLength + 1] == 'x' || format[i + digitLength + 1] == 'X') {
				int data = va_arg(valist, int);
				print_int(data,16);
			} else if (format[i+ digitLength + 1] == 'c') {
				int c = va_arg(valist, int); // va_arg uses int instead of char
				print_char(c);
			} else if (format[i + digitLength + 1] == 's') {
				char* str = va_arg(valist, char*);
				print(str);
			} else if (format[i + digitLength + 1] == '%'){
				print_char('%');
			}
			i += 1 + digitLength;
			continue;
		} else if (format[i] == '\n' || format[i] == '\r') {
			print_char('\n');
		} else {
			print_char(format[i]);
		}
	}

	va_end(valist);
}

// sscanf("info:abc num:123","info:%s num:%d",str,num)
void sscanf(const char* s, const char* format, ...) {
	int narg = 0;
	int i = 0;
	for (i = 0; format[i]; i++)
		if (format[i] == '%')
			narg++;

	va_list valist;
	va_start(valist, format);

	i = 0;
	int s_i = 0;
	int offset;
	for (i = 0; format[i]; ++i) {
		if (format[i] == '%') {
			if (format[i + 1] == 'c') {
				char* pc = va_arg(valist, char*);
				*pc = s[s_i];
				offset = 1;
			} else if (format[i + 1] == 'd') {
				int* pd = va_arg(valist, int*);
				offset = read_int(s+s_i, pd);
			} else if (format[i + 1] == 's') {
				char* pstr = va_arg(valist, char*);
				while (s[s_i] && !isspace(s[s_i])) {
					*pstr = s[s_i];
					pstr++;
					s_i++;
				}
				offset = 0; // s_i has been changed
				*pstr = '\0';
			}
			i += 1;
			s_i += offset;
		} else { // normal match
			if (format[i] == ' ') {
				while (isspace(s[s_i])) {
					s_i++;
				}
			} else if (format[i] == s[s_i]) {
				s_i++;
			} else {
				// printf("not same, %c and %c\n", format[i], s[s_i]);
			}
		}
	}

	va_end(valist);
}

void scanf(const char* format, ...) {
	int narg = 0;
	int i = 0;
	for (i = 0; format[i]; i++)
		if (format[i] == '%')
			narg++;

	va_list valist;
	va_start(valist, format);

	char s[MAX_BUF_LEN];
	getline(s);

	i = 0;
	int s_i = 0;
	int offset;
	for (i = 0; format[i]; ++i) {
		if (format[i] == '%') {
			if (format[i + 1] == 'c') {
				char* pc = va_arg(valist, char*);
				*pc = s[s_i];
				offset = 1;
			} else if (format[i + 1] == 'd') {
				int* pd = va_arg(valist, int*);
				offset = read_int(s+s_i, pd);
			} else if (format[i + 1] == 's') {
				char* pstr = va_arg(valist, char*);
				while (s[s_i] && !isspace(s[s_i])) {
					*pstr = s[s_i];
					pstr++;
					s_i++;
				}
				offset = 0; // s_i has been changed
				*pstr = '\0';
			}
			i += 1;
			s_i += offset;
		} else { // normal match
			if (format[i] == ' ') {
				while (isspace(s[s_i])) {
					s_i++;
				}
			} else if (format[i] == s[s_i]) {
				s_i++;
			} else {
				// printf("not same, %c and %c\n", format[i], s[s_i]);
			}
		}
	}

	va_end(valist);
}

void put_error(char* str)
{
	set_color(RED,BLACK);
	print(str);
	print("\n");
	set_color(WHITE,BLACK);
}

void put_info(char* str)
{
	set_color(WHITE,BLACK);
	print("[ ");
	set_color(GREEN,BLACK);
	print("OK");
	set_color(WHITE,BLACK);
	print(" ] ");
	print(str);
	print("\n");
}

extern void show_static_string(char* str,int row); // assembly

#endif // STDIO_H