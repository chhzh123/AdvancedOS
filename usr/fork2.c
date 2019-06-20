// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "api.h"

void main()
{
	int a = fork();
	int b = fork();
	int c = fork();
	if (a == 0 || b == 0 || c == 0)
		printf("This is hello from process %d! (child)\n", get_pid());
	else
		printf("This is hello from process %d! (parent)\n", get_pid());
	return;
}