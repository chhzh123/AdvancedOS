// Copyright(c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

#ifndef IO_H
#define IO_H

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

unsigned char port_byte_in(unsigned short port){
	unsigned char result;
	__asm__ ("in al, dx"
			:"=a"(result)
			:"d"(port)
			);
	return result;
}

void port_byte_out(unsigned short port, unsigned char data){
	__asm__ ("out dx, al"
			:
			:"a"(data), "d"(port)
			);
}

unsigned short port_word_in(unsigned short port){
	unsigned short result;
	__asm__ ("in ax, dx"
			:"=a"(result)
			:"d"(port)
			);
	return result;
}

void port_word_out(unsigned short port, unsigned short data){
	__asm__ ("out dx, ax"
			:
			:"a"(data), "d"(port)
			);
}

#endif // IO_H