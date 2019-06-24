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
#include "fat12.h"

void exec_user_prg(char* name_list)
{
	disable();
	char* name = name_list, *rest = name_list;
	strsep(&rest," ");

	for (int i = 0; name != NULL; ++i){
		char buf[20];
		strcpy(buf,name);
		char* ext = buf;
		strsep(&ext,".");

		uintptr_t addr_exec;
		memset(bin_img,0,sizeof(bin_img));

		int stack = 0;
		__asm__ volatile ("mov eax, esp":"=a"(stack)::);
		tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);
		if (strcmp(ext,"com") == 0){
			if (strcmp(name,"prg1.com") == 0)
				addr_exec = 0x20000;
			else if (strcmp(name,"prg2.com") == 0)
				addr_exec = 0x30000;
			else if (strcmp(name,"prg3.com") == 0)
				addr_exec = 0x40000;
			else if (strcmp(name,"prg4.com") == 0)
				addr_exec = 0x50000;
			if (!fat12_read_file(name,(char*)addr_exec,NULL)){
				put_error("No this user program!");
				return;
			}
		} else if (strcmp(ext,"out") == 0 && fat12_read_file(name,(char*)bin_img,NULL)) {
			addr_exec = parse_elf(ADDR_USER_START + 6 * PROC_SIZE);
		} else {
			put_error("No this user program!");
			return;
		}

		process* pp = proc_create(USER_CS,USER_DS,addr_exec);
		char info_buf[30];
		sprintf(info_buf,"Created user process %d!",pp->pid);
		put_info(info_buf);

		name = rest;
		strsep(&rest," ");
	}
	// clear_screen();
	schedule_proc();
}

#endif // USER_H