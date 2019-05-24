// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** api.h ******/

#ifndef API_H
#define API_H

// user API
int fork() {
	int pid;
	asm volatile (
		"mov ax, 10\n\t"
		"int 0x80\n\t"
		:"=a"(pid)
		:
		);
	return pid;
}

void wait() {
	asm volatile (
		"mov ax, 11\n\t"
		"int 0x80\n\t"
		:
		:
		);
}

void exit() {
	asm volatile (
		"mov ax, 12\n\t"
		"int 0x80\n\t"
		:
		:
		);
}

#endif // API_H