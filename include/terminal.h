// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** terminal.h ******/

#ifndef TERMINAL_H
#define TERMINAL_H

#include "stdio.h"
#include "string.h"
#include "user.h"

const char* HELLO_INFO = "\
     =====    ||     ||    =======\n\
   //         ||     ||        //\n\
   ||         ||=====||      // \n\
   \\\\         ||     ||    //\n\
     =====    ||     ||    =======\n\
Welcome to CHZOS!\n";
const char* HELP_INFO =
"CHZ OS Shell version 1.0\n\
These shell commands are defined internally. Type 'help' to see this list.\n\
\n\
 help       -- Show this list\n\
 ls         -- Show existing files\n\
 tick       -- Show current execution time\n\
 read       -- Read disk and print out\n\
 sys        -- Test system call in user mode\n\
 exec       -- Create 4 user processes and run all of them\n\
 exec [num] -- Execute the num-th program\n\
 clr        -- Clear the screen\n\
 exit       -- Exit OS\n";

void command_not_found(char* str)
{
	if (strcmp(str,"\n") == 0 || strlen(str) == 0)
		return;
	printf("%s: command not found\n",str);
}

void put_prompt()
{
	set_color(GREEN,BLACK);
	printf("CHZOS:");
	set_color(LIGHT_BLUE,BLACK);
	printf("%s",curr_path);
	set_color(WHITE,BLACK);
	printf("$ ");
}

void terminal_loop()
{
	while (1) {
		put_prompt();
		char str[MAX_BUF_LEN];
		getline(str);
		char* rest = str;
		strsep(&rest," ");
		if (strcmp(str,"help") == 0)
			print(HELP_INFO);
		else if (strcmp(str,"exit") == 0)
			break;
		else if (strcmp(str,"clr") == 0)
			clear_screen();
		else if (strcmp(str,"ls") == 0)
			fat12_ls();
		else if (strcmp(str,"cd") == 0){
			fat12_cd(rest);
		} else if (strcmp(str,"tick") == 0)
			printf("%d\n",get_tick_count());
		else if (strcmp(str,"read") == 0)
			read_disk_test();
		else if (strcmp(str,"exec") == 0){
			exec_user_prg(rest);
		} else
			command_not_found(str);
	}
}

void terminal()
{
	set_color(CYAN,BLACK);
	print(HELLO_INFO);
	set_color(WHITE,BLACK);
	terminal_loop();
}

#endif // TERMINAL_H