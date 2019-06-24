// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** syscall.h ******/

#ifndef SYSCALL_H
#define SYSCALL_H

#include "task.h"
#include "semaphore.h"
#include "sysfile.h"

void terminal_loop();

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

extern void sys_interrupt_handler ();
int sys_interrupt_handler_main (int no) {
#ifdef DEBUG
	printf("Interrupt num:%d\n", no);
#endif
	int arg;
	asm volatile("":"=b"(arg):);
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
	} else if (no == 20) {
		int sem_id = do_getsem(arg); // val
		return sem_id;
	} else if (no == 21) {
		do_sem_p(arg); // sem_id
	} else if (no == 22) {
		do_sem_v(arg); // sem_id
	} else if (no == 23) {
		do_freesem(arg); // sem_id
	} else if (no == 100) {
		curr_proc->status = PROC_TERMINATED;
		curr_proc = NULL; // important!
		enable();
		terminal_loop();
	}
	return 0;
}

extern void sys_file_handler ();
int sys_file_handler_main (int no) {
#ifdef DEBUG
	printf("File interrupt num:%d\n", no);
#endif
	uintptr_t arg1, arg2, arg3, arg4;
	asm volatile("":"=b"(arg1):);
	asm volatile("":"=S"(arg2):);
	asm volatile("":"=d"(arg3):);
	asm volatile("":"=D"(arg4):);
	if (no == 0)
		return (uintptr_t) do_fopen((const char*)arg1, (const char*)arg2);
	else if (no == 1)
		return do_fclose((FILE*)arg1);
	else if (no == 2)
		return do_fread((void*)arg1,(int)arg2,(int)arg3,(FILE *)arg4);
	else if (no == 3)
		return do_fwrite((void*)arg1,(int)arg2,(int)arg3,(FILE *)arg4);
	else if (no == 4)
		return (uintptr_t) do_fgets((char *)arg1, (int)arg2, (FILE *)arg3);
	else if (no == 5)
		return do_fputs((const char *)arg1, (FILE *)arg2);
	return 0;
}

#endif // SYSCALL_H