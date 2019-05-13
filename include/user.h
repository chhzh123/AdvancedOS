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

uintptr_t test_user_function;
// void test_user_function()
// {
// 	int a = 1, b = 1;
// 	printf("a + b = %d + %d = %d\n", a, b, a + b);
// }

void user_mode() {
	int stack = 0;

	__asm__ volatile (
			"mov eax, esp"
			:"=a"(stack)
			:
			);

	tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);

	read_sectors(USER_ADDR,0,2);

	void* new_addr = (void*)NEW_USER_ADDR;
	memcpy(new_addr,(const void*)USER_ADDR,SECTSIZE*2);
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

	put_info("Begin entering user mode...");

	clear_screen();
	test_user_function = NEW_USER_ADDR;
	enter_usermode(test_user_function);

	put_info("Finish user mode!");
}

#endif // USER_H