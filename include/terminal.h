// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** terminal.h ******/

#ifndef TERMINAL_H
#define TERMINAL_H

#include "stdio.h"
#include "string.h"
#include "user.h"

#define MAX_TERMINAL 4
#define VIDEO_SIZE 80 * 32 * 2

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
 help             -- Show this list\n\
 ls               -- Show existing files in current directory\n\
 cd [dir]         -- Change directory\n\
 cp [src] [dst]   -- Copy files\n\
 rm [file]        -- Delete files\n\
 tick             -- Show current execution time\n\
 exec [prg] [...] -- Execute the user program (support multiple programs)\n\
 clr              -- Clear the screen\n\
 exit             -- Exit OS\n";

typedef struct terminal
{
	int num;
	int cursor;
	char buf[VIDEO_SIZE];

} terminal_t;
static terminal_t terminal_list[MAX_TERMINAL];

terminal_t *curr_terminal, *tmp_terminal;

void set_at_first_term()
{
	curr_terminal = tmp_terminal = &terminal_list[0];
	curr_terminal->num = 0;
	curr_terminal->cursor = 0;
}

uintptr_t get_terminal_buf_addr()
{
	if (curr_terminal->num == tmp_terminal->num)
		return VIDEO_ADDRESS;
	else
		return (uintptr_t)&tmp_terminal->buf;
}

void set_cursor(int offset){
	if (curr_terminal->num == tmp_terminal->num){
		offset /= 2; // Convert from cell offset to char offset
		// This is similar to get_cursor, only now we write
		// bytes to those internal device registers
		port_byte_out(REG_SCREEN_CTRL, 14);
		port_byte_out(REG_SCREEN_DATA, (unsigned char)((offset >> 8) & 0x00FF));
		port_byte_out(REG_SCREEN_CTRL, 15);
		port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0x00FF));
	} else
		tmp_terminal->cursor = offset;
}

int get_cursor() {
	if (curr_terminal->num == tmp_terminal->num){
		// The device uses its control register as an index
		// to select its internal registers, of which we are
		// interested in:
		// reg 14: which is the high byte of the cursor's offset
		// reg 15: which is the low byte of the cursor's offset
		// Once the internal register has been selected, we may read or
		// write a byte on the data register
		port_byte_out(REG_SCREEN_CTRL, 14);
		int offset = port_byte_in(REG_SCREEN_DATA) << 8;
		port_byte_out(REG_SCREEN_CTRL, 15);
		offset += port_byte_in(REG_SCREEN_DATA);
		// Since the cursor offset reported by the VGA hardware is the
		// number of characters, we multiply by two to convert it to
		// a character cell offset
		return offset * 2;
	} else
		return tmp_terminal->cursor;
}

void command_not_found(char* str)
{
	if (strcmp(str,"\n") == 0 || strlen(str) == 0)
		return;
	printf("%s: command not found\n",str);
}

void put_prompt()
{
	set_color(GREEN,BLACK);
	printf("root@CHZOS_%d:",curr_terminal->num);
	set_color(LIGHT_BLUE,BLACK);
	printf("%s",curr_path);
	set_color(WHITE,BLACK);
	printf("$ ");
}

void terminal_loop()
{
	while (1) {
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
		} else if (strcmp(str,"rm") == 0){
			fat12_rm(rest);
		} else if (strcmp(str,"write") == 0){
			char buf[1000];
			strcpy(buf,"This is a test message!");
			fat12_create_file((uintptr_t)buf,strlen(buf),"test.txt");
		} else if (strcmp(str,"cp") == 0){
			char* src = rest;
			strsep(&rest," ");
			fat12_cp(src,rest);
		} else if (strcmp(str,"tick") == 0)
			printf("%d\n",get_tick_count());
		else if (strcmp(str,"read") == 0)
			read_disk_test();
		else if (strcmp(str,"exec") == 0){
			exec_user_prg(rest);
		} else
			command_not_found(str);
		put_prompt();
	}
}

void new_terminal()
{
	set_color(CYAN,BLACK);
	print(HELLO_INFO);
	set_color(WHITE,BLACK);
	put_prompt();
}

void change_terminal(int new_ter)
{
	disable();
	memcpy((void*)curr_terminal->buf,(void*)VIDEO_ADDRESS,VIDEO_SIZE);
	curr_terminal->cursor = get_cursor();
	curr_terminal = tmp_terminal = &terminal_list[new_ter];
	clear_screen();
	memcpy((void*)VIDEO_ADDRESS,(void*)curr_terminal->buf,VIDEO_SIZE);
	set_cursor(curr_terminal->cursor);
	enable();
}

void terminal_init()
{
	for (int i = 0; i < MAX_TERMINAL; ++i){
		curr_terminal = tmp_terminal = &terminal_list[i];
		curr_terminal->num = i;
		curr_terminal->cursor = 0;
		memset(curr_terminal->buf,0,sizeof(curr_terminal->buf));
	}
	curr_terminal = tmp_terminal = &terminal_list[0];
	for (int i = 0; i < MAX_TERMINAL; ++i){
		new_terminal();
		change_terminal((i+1)%MAX_TERMINAL);
	}
}

#endif // TERMINAL_H