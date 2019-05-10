// Copyright(c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

#ifndef IO_H
#define IO_H

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

static inline unsigned char port_byte_in(unsigned short port){
	unsigned char result;
	__asm__ volatile ("in al, dx"
					 :"=a"(result)
					 :"d"(port)
					 );
	return result;
}

static inline void port_byte_out(unsigned short port, unsigned char data){
	__asm__ volatile ("out dx, al"
					 :
					 :"a"(data), "d"(port)
					 );
}

static inline unsigned short port_word_in(unsigned short port){
	unsigned short result;
	__asm__ volatile ("in ax, dx"
					 :"=a"(result)
					 :"d"(port)
					 );
	return result;
}

static inline void port_word_out(unsigned short port, unsigned short data){
	__asm__ volatile ("out dx, ax"
					 :
					 :"a"(data), "d"(port)
					 );
}

#endif // IO_H