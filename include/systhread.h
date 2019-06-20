// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** systhread.h ******/

#ifndef SYSTHREAD_H
#define SYSTHREAD_H

#include "task.h"
#include <stdint.h>

void user_pthread_return();

int do_thread_create(int* tid, uintptr_t func, void* args)
{
	disable();
#ifdef DEBUG
	put_info("In pthread_create");
#endif
	process* child;

	// find empty entry
	if ((child = proc_alloc()) == 0) {
		enable();
		return -1; // fail to create child process
	}

	// copy PCB, which has been saved by interrupt
	child->regImg.eip = curr_proc->regImg.eip;
	child->regImg.cs = curr_proc->regImg.cs;
	child->regImg.eflags = curr_proc->regImg.eflags;

	child->regImg.eax = curr_proc->regImg.eax;
	child->regImg.ecx = curr_proc->regImg.ecx;
	child->regImg.edx = curr_proc->regImg.edx;
	child->regImg.ebx = curr_proc->regImg.ebx;

	child->regImg.esp = curr_proc->regImg.esp;
	child->regImg.ebp = curr_proc->regImg.ebp + (child->pid * 0x100); // use different stack!
	child->regImg.esi = curr_proc->regImg.esi;
	child->regImg.edi = curr_proc->regImg.edi;

	child->regImg.ds = curr_proc->regImg.ds;
	child->regImg.es = curr_proc->regImg.es;
	child->regImg.fs = curr_proc->regImg.fs;
	child->regImg.gs = curr_proc->regImg.gs;

	child->regImg.ss = curr_proc->regImg.ss;
	child->regImg.user_esp = curr_proc->regImg.user_esp + (child->pid * 0x100);

	// copy stack
	memcpy((void*)(child->regImg.user_esp),
		(void*)(curr_proc->regImg.user_esp),
		(curr_proc->regImg.ebp - curr_proc->regImg.user_esp)*2); // each entry 32-bit

#ifdef DEBUG
	printf("Size:%d\n", curr_proc->regImg.ebp - curr_proc->regImg.user_esp);
	show_hex(curr_proc->regImg.user_esp,20);
	show_hex(child->regImg.user_esp,20);
#endif

	// set state
	child->regImg.eax = 0;
	child->parent = curr_proc;
	child->status = PROC_READY;
	reset_time(child);

	// set function entrance
	child->regImg.eip = func;
	uintptr_t* stack = (uintptr_t*) child->regImg.user_esp;
	stack--;
	*stack = (uintptr_t) args; // pass arguments
	stack--;
	*stack = (uintptr_t) user_pthread_return; // return address
	child->regImg.user_esp = (uintptr_t) stack;

	// set return values
	*tid = child->pid;

#ifdef DEBUG
	printf("Create light-weight process %d!\n", child->pid);
#endif

	return 0;
}

void do_thread_join(int tid, void** ret)
{
	disable();
#ifdef DEBUG
	put_info("In pthread_join");
#endif
	curr_proc->status = PROC_WAITING;
	schedule_proc();
	enable();
}

int do_thread_self()
{
	return curr_proc->pid;
}

void do_thread_exit()
{
	disable();
#ifdef DEBUG
	put_info("In pthread_exit");
#endif
	curr_proc->status = PROC_TERMINATED;
	if (curr_proc->parent != NULL)
		wakeup(curr_proc->parent->pid);
	enable();
	user_process_return();
}

void user_pthread_return() {
	asm volatile (
		"int 0x81\n\t"
		:
		:"a"(3)
		);
}

extern void sys_pthread_handler ();
int sys_pthread_handler_main (int no) {
#ifdef DEBUG
	printf("Pthread interrupt num:%d\n", no);
#endif
	uintptr_t arg1, arg2, arg3;
	asm volatile("":"=b"(arg1):);
	asm volatile("":"=c"(arg2):);
	asm volatile("":"=d"(arg3):);
	if (no == 0) {
		return do_thread_create((int*) arg1, arg2, (void*) arg3);
	} else if (no == 1) {
		do_thread_join((int) arg1, (void**) arg2);
	} else if (no == 2) {
		return do_thread_self();
	} else if (no == 3) {
		do_thread_exit();
	}
	return 0;
}

#endif // SYSTHREAD_H