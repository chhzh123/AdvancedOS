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

extern void enter_usermode(uintptr_t addr); // assembly

typedef struct Program{
	char name[8];
	int space;
	char pos[8];
	char description[50];
} Program;

#define PRG_NUM 6
#define PrgSectorOffset 0
Program prgs[PRG_NUM];

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
			case 6: strcpy(prgs[i].description,"System call test"); break;
		}
	}
	printf("Name  Size  Pos  Description\n");
	for (int i = 0; i < PRG_NUM; ++i)
		printf("%s %d %s %s\n", prgs[i].name, prgs[i].space, prgs[i].pos, prgs[i].description);
}

void create_one_proc(int num) {
	read_sectors(ADDR_USER_START+(num-1)*PROC_SIZE,(num-1)*2,2);
	proc_create(USER_CS,USER_DS,ADDR_USER_START+(num-1)*PROC_SIZE);
	char str[100];
	sprintf(str,"Created user process %d!",curr_pid);
	put_info(str);
}

void create_user_proc() {

	disable();
	create_one_proc(1);
	create_one_proc(2);
	create_one_proc(3);
	create_one_proc(4);
	enable(); // VERY IMPORTANT!!!

#ifdef DEBUG
	put_info("Finish creating user process!");
#endif
}

void test_system_call() {
	disable();
	create_one_proc(6);
	enable();
}

void exec_user_prg(int num) {
	if (!(num > 0 && num < PRG_NUM+1)){
		put_error("Error: No this program!");
		return;
	}

	// set up kernel stack
	int stack = 0;
	__asm__ volatile ("mov eax, esp":"=a"(stack)::);
	tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);

	read_sectors(ADDR_USER_START,(num-1)*2,2);

#ifdef DEBUG
	show_one_sector(ADDR_USER_START);
#endif

	put_info("Begin entering user mode...");

#ifndef DEBUG
	clear_screen();
#endif
	enter_usermode((uintptr_t)ADDR_USER_START);

	put_info("Finish user mode!");
}

#endif // USER_H