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
 tick       -- Show current execution time\n\
 read       -- Read disk and print out\n\
 user       -- Enter user mode\n\
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

void read_disk(){
	printf("Please enter the sector number: ");
	int num;
	scanf("%d",&num);
	printf("Reading sector %d...\n", num);
	uint8_t* sector = flpydsk_read_sector(num);
	// display sector
	if (sector != 0) {
		int i = 0;
		for (int c = 0; c < 1; c++ ) {
			for (int j = 0; j < 128; j++){
				printf ("%x", sector[ i + j ]);
				if (j % 2 == 1)
					printf(" ");
			}
			i += 128;
		}
		printf("\n");
	}
	else
		put_error("Error reading sector from disk");
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
		else if (strcmp(str,"tick") == 0)
			printf("%d\n",get_tick_count());
		else if (strcmp(str,"read") == 0)
			read_disk();
		else if (strcmp(str,"user") == 0)
			user_mode();
		else
			command_not_found(str);
	}
}

#endif // TERMINAL_H