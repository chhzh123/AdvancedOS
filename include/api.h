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

// semaphore
int get_sem(int val) {
	int sem_id;
	asm volatile (
		"mov eax, 20\n\t"
		"int 0x80\n\t"
		:"=a"(sem_id)
		:"b"(val)
		);
	return sem_id;
}

void sem_wait(int sem_id) {
	asm volatile (
		"mov eax, 21\n\t"
		"int 0x80\n\t"
		:
		:"b"(sem_id)
		);
}

void sem_signal(int sem_id) {
	asm volatile (
		"mov eax, 22\n\t"
		"int 0x80\n\t"
		:
		:"b"(sem_id)
		);
}

void free_sem(int sem_id) {
	asm volatile (
		"mov eax, 23\n\t"
		"int 0x80\n\t"
		:
		:"b"(sem_id)
		);
}

#endif // API_H