// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "api.h"

int tot;
int mutex;
int n;

void main() {
	tot = 0;
	mutex = get_sem(1);
	n = get_sem(0);
	int pid = fork();
	if (pid == -1) {
		printf("error in fork!");
		exit(-1);
	}
	if (pid) {
		while (1) { // Producer
			sem_wait(mutex);
			printf("Produce %d\n", ++tot);
			sem_signal(mutex);
			sem_signal(n);
		}
	} else {
		while (1) { // Consumer
			sem_wait(n);
			sem_wait(mutex);
			printf("Consume %d\n", tot--);
			sem_signal(mutex);
		}
	}
}