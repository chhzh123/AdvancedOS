// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** terminal.h ******/

#ifndef TERMINAL_H
#define TERMINAL_H

#include "stdio.h"
#include "string.h"

const char* PROMPT_INFO = "chzos> ";
const char* HELLO_INFO = "Welcome to CHZOS!\n";
const char* HELP_INFO =
"CHZ OS Shell version 0.5\n\
These shell commands are defined internally. Type 'help' to see this list.\n\
\n\
 help       -- Show this list\n\
 show       -- Show existing programs\n\
 inter      -- Execute the C / Python Interpreter\n\
 exec       -- Execute all the user programs\n\
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
	print(PROMPT_INFO);
	set_color(WHITE,BLACK);
}

void terminal()
{
	set_color(CYAN,BLACK);
	print(HELLO_INFO);
	set_color(WHITE,BLACK);
	while (1) {
		put_prompt();
		char str[MAX_BUF_LEN];
		getline(str);
		if (strcmp(str,"help") == 0)
			print(HELP_INFO);
		else if (strcmp(str,"exit") == 0)
			break;
		else if (strcmp(str,"clr") == 0)
			clear_screen();
		else
			command_not_found(str);
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
	}
}

#endif // TERMINAL_H