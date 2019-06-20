// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** pthread.h ******/

#ifndef PTHREAD_H
#define PTHREAD_H

int pthread_create(int* tid, uintptr_t func, void* args) {
	int ret;
	asm volatile (
		"int 0x81\n\t"
		:"=a"(ret)
		:"a"(0),"b"(tid),"c"(func),"d"(args)
		);
	return ret;
}

void pthread_join(int tid, void** ret) {
	asm volatile (
		"int 0x81\n\t"
		:
		:"a"(1),"b"(tid),"c"(ret)
		);
}

int pthread_self() {
	int tid;
	asm volatile (
		"int 0x81\n\t"
		:"=a"(tid)
		:"a"(2)
		);
	return tid;
}

void pthread_exit() {
	asm volatile (
		"int 0x81\n\t"
		:
		:"a"(3)
		);
}

#endif // PTHREAD_H