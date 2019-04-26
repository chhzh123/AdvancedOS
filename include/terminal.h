// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** terminal.h ******/

#ifndef TERMINAL_H
#define TERMINAL_H

#include "stdio.h"

const char* PROMPT_INFO = "chzos> ";
const char* HELLO_INFO = "Welcome to CHZOS!\n";
const char* HELP_INFO =
"CHZ OS Shell version 0.1\n\
These shell commands are defined internally. Type 'help' to see this list.\n\
\n\
 help       -- Show this list\n\
 show       -- Show existing programs\n\
 inter      -- Execute the C / Python Interpreter\n\
 exec       -- Execute all the user programs\n\
 exec [num] -- Execute the num-th program\n\
 clr        -- Clear the screen\n\
 exit       -- Exit OS\n";

// void command_not_found(char* str)
// {
// 	char* newline = "\n";
// 	if (strcmp(str,newline) == 0 || strlen(str) == 0)
// 		return;
// 	strcat(str,CMD_NOT_FOUND);
// 	puts(str);
// 	puts(newline);
// }

void put_prompt()
{
	set_color(GREEN,BLACK);
	print(PROMPT_INFO);
	set_color(WHITE,BLACK);
}

void terminal()
{
	set_color(CYAN,BLACK);
	print(HELLO_INFO);
	set_color(WHITE,BLACK);
	while (1){
		put_prompt();
		char str[MAX_BUF_LEN];
		getline(str);
		while (1) {}
		// if (strcmp(str,"help") == 0)
		// 	print(HELP_INFO);
		// else if (strcmp(str,SHOW_STR) == 0)
		// 	show_prg_info();
		// else if (strcmp(str,EXIT_STR) == 0)
		// 	break;
		// else if (strcmp(str,CLR_STR) == 0)
		// 	clear();
		// else if (strcmp(str,INTER_STR) == 0)
		// 	interpreter();
		// else if (strlen(str) >= 4){
		// 	char cpystr[MAX_BUF_LEN];
		// 	strncpy(cpystr,str,4);
		// 	if (strcmp(cpystr,EXE_STR) == 0){
		// 		clear();
		// 		if (strlen(str) == 4){
		// 			for (size_os i = 1; i < 5; ++i)
		// 				execute(i+'0'); // batch execution
		// 		} else{
		// 			execute(str[5]);
		// 		}
		// 		clear();
		// 	} else
		// 		command_not_found(str);
		// } else
		// 	command_not_found(str);
	}
}

#endif // TERMINAL_H