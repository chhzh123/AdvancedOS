// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "api.h"
#include "string.h"

char str[80] = "129djwqhdsajd128dw9i39ie93i8494urjoiew98kdkd";
int LetterNr = 0;

void CountLetter()
{
	int len = strlen(str);
	for(int i = 0; i < len; ++i)
		if(isalpha(str[i]))
			LetterNr++;
}

void main() {
	int pid = fork();
	if (pid == -1){
		printf("Error in fork!\n");
		return;
	}
	if (pid) {
		wait();
		printf("LetterNr = %d, I'm parent!\n", LetterNr);
	} else {
		CountLetter();
		printf("I'm child!\n");
		exit(0);
	}
	return;
}