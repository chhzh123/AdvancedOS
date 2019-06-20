// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "api.h"

int sem;
int bank_account = 1000;

void main() {
	sem = get_sem(1);
	int pid = fork();
	if (pid == -1) {
		printf("error in fork!");
		exit(-1);
	}
	if (pid) { // parent
		for (int i = 0; i < 100; ++i) {
			sem_wait(sem);
			bank_account += 10;
			printf("Father: deposite $10 Account: $%d\n", bank_account);
			sem_signal(sem);
		}
	} else { // child
		for (int i = 0; i < 50; ++i) {
			sem_wait(sem);
			bank_account -= 20;
			printf("Child: withdraw $20 Account: $%d\n", bank_account);
			sem_signal(sem);
		}
		exit(0);
	}
	wait(); // parent
	return;
}