// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** syscall.h ******/

#ifndef SYSCALL_H
#define SYSCALL_H

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

void sys_interrupt_handler_main (int no) {
	if (no == 0)
		printf("%s",LOGO);
	else {
		put_info("Sleep for 100ms");
		int cnt = 0;
		while (cnt++ < 10000); // cannot use sleep, or will cause nested interrupt
		put_info("Done sleep!");
	}
}

#endif // SYSCALL_H