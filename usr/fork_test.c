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
	// int pid;
	asm volatile ("mov eax,0\n\tint 0x80\n\t"); // not ax, but eax!!!
	while(1){}
	// pid = fork();
	// if (pid == -1)
	// 	printf("Error in fork!\n");
	// if (pid) {
	// 	wait();
	// 	printf("LetterNr = %d, pid = %d\n", LetterNr, pid);
	// } else {
	// 	CountLetter();
	// 	printf("Child pid = %d\n", pid);
	// 	exit(0);
	// }
}