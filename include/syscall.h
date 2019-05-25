// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** syscall.h ******/

#ifndef SYSCALL_H
#define SYSCALL_H

#include "task.h"

void terminal_loop();
extern void sys_interrupt_handler ();

const char* LOGO = "\
     =====    ||     ||    =======\n\
   //         ||     ||        //\n\
   ||         ||=====||      // \n\
   \\\\         ||     ||    //\n\
     =====    ||     ||    =======\n";

void sleep (int ms) {
	int ticks = ms + get_tick_count ();
	while (ticks > get_tick_count ());
}

int sys_interrupt_handler_main (int no) {
#ifdef DEBUG
	printf("Interrupt num:%d\n", no);
#endif
	if (no == 0) {
		printf("%s",LOGO);
	} else if (no == 1) {
		put_info("Sleep for 100ms");
		int cnt = 0;
		while (cnt++ < 10000); // cannot use sleep, or will cause nested interrupt
		put_info("Done sleep!");
	} else if (no == 10) {
		int pid = do_fork();
		return pid;
	} else if (no == 11) {
		do_wait();
	} else if (no == 12) {
		do_exit();
	} else if (no == 13) {
		return sys_get_pid();
	} else if (no == 100) {
		enable();
		terminal_loop();
	}
	return 0;
}

#endif // SYSCALL_H