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
#include "task.h"
#include "elf.h"

extern void enter_usermode(uintptr_t addr); // assembly

typedef struct Program{
	char name[8];
	int space;
	char pos[8];
	char description[50];
} Program;

#define PRG_NUM 11
#define PrgSectorOffset 0
Program prgs[PRG_NUM];

static char info_buf[100];

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
			case 7: strcpy(prgs[i].description,"Fork test"); break;
			case 8: strcpy(prgs[i].description,"Fork test 2"); break;
			case 9: strcpy(prgs[i].description,"Bank account");break;
			case 10: strcpy(prgs[i].description,"Fruit");break;
			case 11: strcpy(prgs[i].description,"Producer-consumer");break;
		}
	}
	printf("Name  Size  Pos  Description\n");
	for (int i = 0; i < PRG_NUM; ++i)
		printf("%s %d %s %s\n", prgs[i].name, prgs[i].space, prgs[i].pos, prgs[i].description);
}

void create_one_proc(int num) {
	read_sectors(ADDR_USER_START+(num-1)*PROC_SIZE,(num-1)*2,2);
	process* pp = proc_create(USER_CS,USER_DS,ADDR_USER_START+(num-1)*PROC_SIZE);
	sprintf(info_buf,"Created user process %d!",pp->pid);
	put_info(info_buf);
}

void create_user_proc() {

	disable();
	create_one_proc(1);
	create_one_proc(2);
	create_one_proc(3);
	create_one_proc(4);
	schedule_proc();

#ifdef DEBUG
	put_info("Finish creating user process!");
#endif
}

void test_system_call() {
	disable();
	create_one_proc(6);
	schedule_proc();
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

	uintptr_t addr = ADDR_USER_START+(num-1)*PROC_SIZE;
	if (num < 7)
		read_sectors(addr,(num-1)*2,2);
	else
		read_sectors(addr,(num-1)*2,15);

#ifdef DEBUG
	show_one_sector(addr);
#endif

	put_info("Begin entering user mode...");

#ifndef DEBUG
	clear_screen();
#endif
	enter_usermode(addr);

	put_info("Finish user mode!");
}

static uint8_t bin_img[30 * 512];

void exec_elf(int num) {
	disable();
	if (!(num > 0 && num < PRG_NUM+1)){
		put_error("Error: No this program!");
		return;
	}
	memset(bin_img,0,sizeof(bin_img));

	uintptr_t addr_exec;
	addr_exec = ADDR_USER_START + 6 * PROC_SIZE;
	uintptr_t addr = (uintptr_t)bin_img;

	read_sectors(addr,(num-1)*2+(num-7)*28,30);

	// parse elf header
	elfhdr eh;
	memcpy((void*)&eh,(void*)addr,sizeof(elfhdr));
#ifdef DEBUG
	print_elfhdr(&eh);
	show_one_sector(addr);
#endif

	if (eh.e_magic != ELF_MAGIC){
		put_error("Bad elf file!");
		for(;;){}
	}

	// parse program header
	prghdr ph;
	for (int i = 0, offset = eh.e_phoff;
			i < eh.e_phnum; i++, offset += eh.e_phentsize){
		memcpy((void*)&ph,(void*)(addr+offset),eh.e_phentsize);
#ifdef DEBUG
		print_prghdr(&ph);
#endif
		if (i == 0)
			memcpy((void*)addr_exec,(void*)(addr+ph.p_offset),ph.p_memsz);
	}

	process* pp = proc_create(USER_CS,USER_DS,eh.e_entry);
	sprintf(info_buf,"Created user process %d!",pp->pid);
	put_info(info_buf);
	schedule_proc();
}

#endif // USER_H