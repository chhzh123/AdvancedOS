// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** task.h ******/

#ifndef TASK_H
#define TASK_H

#include "stdio.h"
#include "gdt.h"
#include "tss.h"
#include <stdint.h>

/*
0x00000000-0x00400000 – Kernel reserved
0x00400000-0x80000000 – User land
0x80000000-0xffffffff – Kernel reserved
*/
// #define ADDR_USER_START   0x00400000
#define ADDR_USER_START   0x0020000
// #define ADDR_KERNEL_START 0x80000000
// #define PROC_SIZE 0x10000 // 64k
#define PROC_SIZE 0x400 // 1k

#define PRIOR_SYS  0x2
#define PRIOR_USER 0x1

#define MAX_PROCESS 5
#define MAX_TICK 100

enum PROC_STATUS
{
	PROC_SLEEP = 0,
	PROC_ACTIVE = 1
};

struct regs {
	// segment register
	uint32_t gs; // 16-bit
	uint32_t fs; // 16-bit
	uint32_t es; // 16-bit
	uint32_t ds; // 16-bit

	// general registers, saved by `pusha`
	// the order should be the same
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp; // stack
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	// saved by `int` (interrupt)
	uint32_t eip;
	uint32_t cs; // 16-bit
	uint32_t eflags;
	uint32_t user_esp;
	uint32_t ss; // 16-bit
} __attribute__((packed));
typedef struct regs regs;

typedef struct process {
	regs   regImg;
	int    pid;
	int    priority;
	int    status;
	int    tick;
} process;

process proc_list[MAX_PROCESS];
process* curr_proc = NULL;
uint32_t curr_pid = 0;

process* get_proc()
{
	return curr_proc;
}

void reset_time(process* pp)
{
	pp->tick = MAX_TICK;
}

process* proc_alloc()
{
	// process* pp;
	// for (pp = &proc_list[0]; pp < &proc_list[MAX_PROCESS]; ++pp){ // find free process

	// 	if (pp->status == PROC_SLEEP){
	// 		pp->status = PROC_ACTIVE;
	// 		pp->pid = curr_pid++;
	// 		pp->regImg.esp = pp->regImg.ebp = (uintptr_t)ADDR_USER_START + PROC_SIZE;
	// 		pp->tick = MAX_TICK;
	// 		return pp;
	// 	}
	// }
	// return NULL;
	process* pp = &proc_list[curr_pid];
	pp->pid = curr_pid++;
	pp->tick = MAX_TICK;
	return pp;
}

process* proc_create(uint32_t cs, uint32_t ds, uintptr_t addr)
{
	disable();
	process* pp = proc_alloc();
	pp->regImg.cs = cs;
	pp->regImg.ds = ds;
	pp->regImg.es = pp->regImg.ds;
	pp->regImg.fs = pp->regImg.ds;
	pp->regImg.gs = pp->regImg.ds;
	pp->regImg.ss = pp->regImg.ds;
	uint32_t flag;
	asm volatile ("pushf\n\t"
				  "pop eax\n\t"
				  :"=a"(flag)
				  :
				 );
	pp->regImg.eflags = flag | 0x200; // interrupt
	pp->regImg.eip = addr;
	pp->regImg.esp
		= pp->regImg.ebp
		= pp->regImg.user_esp
		= addr + PROC_SIZE; // reset
	reset_time(pp);
#ifdef DEBUG
	printf("Create process %d!\n", curr_pid);
#endif
	return pp;
}

void proc_init()
{
	memset(proc_list,0,sizeof(process) * MAX_PROCESS);
	for (int i = 0; i < MAX_PROCESS; ++i){
		proc_list[i].pid = 0;
		proc_list[i].status = PROC_SLEEP;
		proc_list[i].priority = PRIOR_USER;
	}

	// initialize the first process
	// curr_proc = proc_create(USER_CS,USER_DS,ADDR_USER_START);
}

process* proc_pick()
{
	if (curr_pid == 0)
		return NULL;
	int cnt = 0;
	for (int i = 0; i < curr_pid; ++i){
		if (proc_list[i].status == PROC_ACTIVE)
			return &proc_list[(i+1) % curr_pid];
		else
			cnt++;
	}
	if (cnt == curr_pid)
		return &proc_list[0];
	return NULL;
}

extern void save_proc_entry(); // assembly

// be careful of the order!!!
__attribute__((__cdecl__))
extern void restart_proc(
	uint32_t ds,
	uint32_t eip,
	uint32_t cs,
	uint32_t eflags,
	uint32_t user_esp,
	uint32_t ss
	);

void enter_usermode(uintptr_t addr);

void proc_switch(process* pp)
{
	if (!(curr_proc == NULL && curr_pid == 1)){
		curr_proc->status = PROC_SLEEP;
	}
#ifdef DEBUG
	put_info("In proc_switch");
#else
	else
		clear_screen(); // the first process
#endif
	reset_time(pp);

	pp->status = PROC_ACTIVE;
	curr_proc = pp;
#ifdef DEBUG
	printf("CS:%xh DS:%xh ES:%xh FS:%xh GS:%xh SS:%xh\n",
		pp->regImg.cs, pp->regImg.ds, pp->regImg.es,
		pp->regImg.fs, pp->regImg.gs, pp->regImg.ss);
	printf("EIP:%xh EFLAGS:%xh USER_ESP:%xh EBP:%xh\n",
		pp->regImg.eip, pp->regImg.eflags,
		pp->regImg.user_esp, pp->regImg.ebp);
#endif
	int stack = 0;
	__asm__ volatile ("mov eax, esp":"=a"(stack)::);
	tss_set_stack(KERNEL_DS,stack+KERNEL_STACK_SIZE);

	interruptdone(0);
	enable();
	restart_proc(
		pp->regImg.ds,
		// pp->regImg.eip,
		ADDR_USER_START,
		pp->regImg.cs,
		pp->regImg.eflags,
		pp->regImg.user_esp,
		pp->regImg.ss
		);

#ifdef DEBUG
	char str[10];
	strcpy(str,"Done switch!");
	show_static_string(str,20);
#endif
}

void schedule_proc() // round-robin
{
	process* pp = proc_pick();
	if (pp == NULL)
		return;

	proc_switch(pp);
}

// pit timer interrupt handler
void pit_handler_main(
	uint32_t gs,uint32_t fs,uint32_t es,uint32_t ds,
	uint32_t di,uint32_t si,uint32_t bp,uint32_t sp,
	uint32_t bx,uint32_t dx,uint32_t cx,uint32_t ax,
	uint32_t ip,uint32_t cs,uintptr_t flags)
{
	// increment tick count
	pit_ticks++;

#ifdef DEBUG
	char str[10];
	itoa(pit_ticks,str,10);
	show_static_string(str,24);
#endif

	if (curr_proc == NULL){
		if (curr_pid == 1)
			schedule_proc(); // no need to save
		interruptdone(0);
		return;
	}

#ifdef DEBUG
	itoa(curr_proc->pid,str,10);
	char str2[50];
	strcpy(str2,"Curr proc: ");
	strcat(str2,str);
	strcat(str2," time: ");
	itoa(curr_proc->tick,str,10);
	strcat(str2,str);
	show_static_string(str2,22);
#endif

	if (curr_proc->tick > 0){
		curr_proc->tick--;
		interruptdone(0);
		return;
	}

#ifdef DEBUG
	put_info("In save_proc");
#endif
	curr_proc->regImg.eip = ip+1;
	curr_proc->regImg.cs = cs;
	curr_proc->regImg.eflags = flags;

	curr_proc->regImg.eax = ax;
	curr_proc->regImg.ecx = cx;
	curr_proc->regImg.edx = dx;
	curr_proc->regImg.ebx = bx;

	curr_proc->regImg.esp = sp + 0xc; // 12
	curr_proc->regImg.ebp = bp;
	curr_proc->regImg.esi = si;
	curr_proc->regImg.edi = di;

	curr_proc->regImg.ds = ds & 0xFFFF;
	curr_proc->regImg.es = es & 0xFFFF;
	curr_proc->regImg.fs = fs & 0xFFFF;
	curr_proc->regImg.gs = gs & 0xFFFF;

	curr_proc->regImg.ss = curr_proc->regImg.ds;
	curr_proc->regImg.user_esp = curr_proc->regImg.esp;
#ifdef DEBUG
	printf("CS:%xh DS:%xh ES:%xh FS:%xh GS:%xh SS:%xh\n",
		cs, ds, es, fs, gs, ds);
	printf("EIP:%xh EFLAGS:%xh USER_ESP:%xh EBP:%xh\n",
		ip, flags, sp, bp);
#endif

	schedule_proc(); // change another process

	// tell hal we are done
	interruptdone(0);
}

#endif // TASK_H