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
		"mov eax, 10\n\t"
		"int 0x80\n\t"
		:"=a"(pid)
		:
		);
	return pid;
}

void wait() {
	asm volatile (
		"mov eax, 11\n\t"
		"int 0x80\n\t"
		:
		:
		);
}

void exit() {
	asm volatile (
		"mov eax, 12\n\t"
		"int 0x80\n\t"
		:
		:
		);
}

int get_pid() {
	int pid;
	asm volatile (
		"mov eax, 13\n\t"
		"int 0x80\n\t"
		:"=a"(pid)
		:
		);
	return pid;
}

#endif // API_H