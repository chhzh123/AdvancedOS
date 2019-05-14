// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** user.h ******/

#ifndef USER_H
#define USER_H

#include "stdio.h"
#include "tss.h"
#include "ide.h"
#include "string.h"

#define USER_ADDR 0x1000
#define NEW_USER_ADDR 0x20000
#define KERNEL_STACK_SIZE 2048       // Use a 2kb kernel stack

extern void enter_usermode(uintptr_t addr); // assembly

typedef struct Program{
	char name[8];
	int space;
	char pos[8];
	char description[50];
} Program;

#define PRG_NUM 5
#define PrgSectorOffset 0
Program prgs[PRG_NUM];

uintptr_t test_user_function;

void show_user_prg(){
	for (int i = 0; i < PRG_NUM; ++i){
		char str[10];
		itoa(i+1,str,10);
		strcpy(prgs[i].name,str);
		prgs[i].space = 512;
		strcpy(prgs[i].pos,"/");
		switch (i+1){
			case 1: strcpy(prgs[i].description,"Quadrant 1: Flying single char"); break;
			case 2: strcpy(prgs[i].description,"Quadrant 2: Flying two chars - V shape"); break;
			case 3: strcpy(prgs[i].description,"Quadrant 3: Flying two chars - OS"); break;
			case 4: strcpy(prgs[i].description,"Quadrant 4: Flying two chars - parallelogram"); break;
			case 5: strcpy(prgs[i].description,"Draw the box"); break;
		}
	}
	printf("Name  Size  Pos  Description\n");
	for (int i = 0; i < PRG_NUM; ++i)
		printf("%s %d %s %s\n", prgs[i].name, prgs[i].space, prgs[i].pos, prgs[i].description);
}

void exec_user_prg(int num) {
	if (!(num > 0 && num < PRG_NUM+1)){
		put_error("Error: No this program!");
		return;
	}

	int stack = 0;

	__asm__ volatile (
			"mov eax, esp"
			:"=a"(stack)
			:
			);

	tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);

	read_sectors(USER_ADDR,num*2,2);

	void* new_addr = (void*)NEW_USER_ADDR;
	memcpy(new_addr,(const void*)USER_ADDR,SECTSIZE*2);

#ifdef DEBUG
	uint8_t* sector = (uint8_t*)new_addr;
	int i = 0;
	for (int c = 0; c < 4; c++ ) {
		for (int j = 0; j < 128; j++){
			printf ("%x", sector[ i + j ]);
			if (j % 2 == 1)
				printf(" ");
		}
		i += 128;
	}
	printf("\n");
#endif

	put_info("Begin entering user mode...");

	clear_screen();
	test_user_function = NEW_USER_ADDR;
	enter_usermode(test_user_function);

	put_info("Finish user mode!");
}

#endif // USER_H