// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** kernel.c ******/

#include "hal.h"
#include "task.h"
#include "semaphore.h"
#include "fat12.h"
#include "terminal.h"

void initialize()
{
	/*
	 * Hardware Abstraction Layer (HAL) initialization
	 */
	hal_initialize();

	/*
	 * Process initialization
	 */
	proc_init();
	put_info("Initialized process");

	/*
	 * Semaphore initialization
	 */
	sem_init();
	put_info("Initialized semaphore");

	/*
	 * File system (FAT12) initialization
	 */
	fat12_init();
	put_info("Initialized fat12");

	// set up kernel stack
	int stack = 0;
	__asm__ volatile ("mov eax, esp":"=a"(stack)::);
	tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);

	uintptr_t addr = 0x0020000;
	fat12_read_file("PRG1.COM",(char*)addr);
	show_one_sector(addr);
	enter_usermode(addr);
	put_info("done!");
	while(1){}
}

void main () {
	initialize();
	clear_screen();
	terminal();
}