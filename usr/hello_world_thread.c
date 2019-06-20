// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "pthread.h"

int m = 5;

void hello(void* args){
	char* str = (char*) args;
	for(int i = 0; i < m; i++)
		printf("%s\n", str);
}

void main() {
	int tid1, tid2;
	pthread_create(&tid1,(uintptr_t)hello,"Hello");
	pthread_create(&tid2,(uintptr_t)hello,"world!");
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	return;
}