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
#define PROC_SIZE 0x10000 // 64k

#define PRIOR_SYS  0x2
#define PRIOR_USER 0x1

#define MAX_PROCESS 10
#define MAX_TICK 1

/*
 * Five-state process model
 *
 *                <----------timer------------
 *               /                            \
 * NEW -fork-> READY -------restart-------> RUNNING -exit-> TERMINATED
 *               \                            /
 *                <-wakeup- WAITING <-blocked-
 */

enum PROC_STATUS
{
	PROC_NEW = 0,
	PROC_READY = 1,
	PROC_RUNNING = 2,
	PROC_WAITING = 3, // blocked
	PROC_TERMINATED = 4 // exit
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

/*
 * Reference to Linux
 * https://stackoverflow.com/questions/807506/threads-vs-processes-in-linux
 * View threads as light-weight processes
 */
typedef struct process {
	regs   regImg;
	int    pid;
	struct process* parent;
	struct process* next; // used for semaphore
	int    priority;
	int    status;
	int    tick;
} process;

process proc_list[MAX_PROCESS];
process* curr_proc = NULL;
uint32_t curr_pid = 0;

void print_proc_info(process* pp)
{
	printf("CS:%xh DS:%xh ES:%xh FS:%xh GS:%xh SS:%xh\n",
		pp->regImg.cs, pp->regImg.ds, pp->regImg.es,
		pp->regImg.fs, pp->regImg.gs, pp->regImg.ss);
	printf("EIP:%xh EFLAGS:%xh USER_ESP:%xh EBP:%xh EAX:%xh\n",
		pp->regImg.eip, pp->regImg.eflags,
		pp->regImg.user_esp, pp->regImg.ebp, pp->regImg.eax);
}

process* get_proc()
{
	return curr_proc;
}

void reset_time(process* pp)
{
	pp->tick = MAX_TICK;
}

int sys_get_pid()
{
	if (curr_proc == NULL)
		return -1;
	else
		return curr_proc->pid;
}

void proc_init()
{
	memset(proc_list,0,sizeof(process) * MAX_PROCESS);
	for (int i = 0; i < MAX_PROCESS; ++i){
		proc_list[i].pid = 0;
		proc_list[i].status = PROC_NEW;
		proc_list[i].priority = PRIOR_USER;
		proc_list[i].parent = NULL;
		proc_list[i].next = NULL;
	}
}

// NEW -> READY
process* proc_alloc()
{
	for (int i = 0; i < MAX_PROCESS; ++i){ // find free process
		process* pp = &proc_list[i];
		if (pp->status == PROC_NEW){
			pp->status = PROC_READY;
			pp->pid = ++curr_pid; // start from 1
			return pp;
		}
	}
	return NULL;
}

process* proc_create(uint32_t cs, uint32_t ds, uintptr_t addr)
{
	disable();
	process* pp = proc_alloc();
	pp->regImg.cs = cs;
	pp->regImg.ds
		= pp->regImg.es
		= pp->regImg.fs
		= pp->regImg.gs
		= pp->regImg.ss
		= ds;
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
	printf("Create process %d!\n", pp->pid);
#endif
	return pp;
}

process* proc_pick()
{
	int curr_index = -1;
	// find current running process
	for (int i = 0; i < MAX_PROCESS; ++i){
		if (proc_list[i].status == PROC_RUNNING){
			curr_index = i;
			break;
		}
	}
	// round-robin
	for (int i = 0; i < MAX_PROCESS; ++i){
		int index = (i + 1 + curr_index) % MAX_PROCESS;
		if (proc_list[index].status == PROC_READY)
			return &proc_list[index];
	}
	if (curr_index != -1) // no choice but the process still running
		return &proc_list[curr_index];
	else
		return NULL;
}

// be careful of the order!!!
__attribute__((__cdecl__))
extern void restart_proc(
	uint32_t gs, uint32_t fs, uint32_t es, uint32_t ds,
	uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp,
	uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax,
	uint32_t eip,
	uint32_t cs,
	uint32_t eflags,
	uint32_t user_esp,
	uint32_t ss
	);

void enter_usermode(uintptr_t addr);

void save_proc(
	uint32_t gs,uint32_t fs,uint32_t es,uint32_t ds,
	uint32_t di,uint32_t si,uint32_t bp,uint32_t sp,
	uint32_t bx,uint32_t dx,uint32_t cx,uint32_t ax,
	uint32_t ip,uint32_t cs,uintptr_t flags,
	uint32_t user_esp, uint32_t ss)
{
	if (curr_proc == NULL)
		return;

#ifdef DEBUG
	put_info("In save_proc");
#endif
	curr_proc->regImg.eip = ip;
	curr_proc->regImg.cs = cs;
	curr_proc->regImg.eflags = flags;

	curr_proc->regImg.eax = ax;
	curr_proc->regImg.ecx = cx;
	curr_proc->regImg.edx = dx;
	curr_proc->regImg.ebx = bx;

	curr_proc->regImg.esp = sp; 
	curr_proc->regImg.ebp = bp;
	curr_proc->regImg.esi = si;
	curr_proc->regImg.edi = di;

	curr_proc->regImg.ds = ds & 0xFFFF;
	curr_proc->regImg.es = es & 0xFFFF;
	curr_proc->regImg.fs = fs & 0xFFFF;
	curr_proc->regImg.gs = gs & 0xFFFF;

	curr_proc->regImg.ss = ss;
	curr_proc->regImg.user_esp = user_esp;
#ifdef DEBUG
	print_proc_info(curr_proc);
#endif
}

// READY -> RUNNING && RUNNING -> READY
void proc_switch(process* pp)
{
	if (curr_pid != 0){
		if (curr_proc == NULL) // just created
			{/* do nothing*/}
		else if (curr_proc->status == PROC_RUNNING)
			curr_proc->status = PROC_READY; // avoid PROC_WAITING
	}
#ifdef DEBUG
	put_info("In proc_switch");
#else
	else
		clear_screen(); // the first process
#endif
	reset_time(pp);

	pp->status = PROC_RUNNING;
	curr_proc = pp;
#ifdef DEBUG
	print_proc_info(pp);
#endif
	// set up kernel stack
	int stack = 0;
	__asm__ volatile ("mov eax, esp":"=a"(stack)::);
	tss_set_stack(KERNEL_DS,stack);

	interruptdone(0); // IMPORTANT!!! Enable interrupts to function well
	restart_proc(
		pp->regImg.gs, pp->regImg.fs, pp->regImg.es, pp->regImg.ds,
		pp->regImg.edi, pp->regImg.esi, pp->regImg.ebp, pp->regImg.esp,
		pp->regImg.ebx, pp->regImg.edx, pp->regImg.ecx, pp->regImg.eax,
		pp->regImg.eip,
		pp->regImg.cs,
		pp->regImg.eflags,
		pp->regImg.user_esp,
		pp->regImg.ss
		);

#ifdef DEBUG // should not return to here!
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
	uint32_t ip,uint32_t cs,uintptr_t flags,
	uint32_t user_esp, uint32_t ss)
{
	// increment tick count
	pit_ticks++;

#ifdef DEBUG
	char str[100];
	itoa(pit_ticks,str,10);
	show_static_string(str,24);
#endif

	if (curr_proc == NULL){
		if (curr_pid != 0) // directly schedule after creating
			schedule_proc(); // no need to save
		interruptdone(0);
		return;
	}

	if (curr_proc->tick > 0){
		curr_proc->tick--;
		interruptdone(0);
		return;
	}

	// save process
	save_proc(gs,fs,es,ds,di,si,bp,sp,bx,dx,cx,ax,ip,cs,flags,user_esp,ss);

	// schedule process
	schedule_proc(); // change another process

	// tell hal we are done
	interruptdone(0);
}

extern uint32_t read_eip();

// NEW -> READY
int do_fork() {

	disable();
#ifdef DEBUG
	put_info("In fork");
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

#ifdef DEBUG
	printf("Create child process %d!\n", child->pid);
#endif

	return child->pid;
}

// initiative, WAITING -> READY
void wakeup(uint8_t pid) {
	disable();
#ifdef DEBUG
	put_info("In wakeup");
#endif
	for (process* pp = &proc_list[0]; pp < &proc_list[MAX_PROCESS]; ++pp){
		if (pp->pid == pid && pp->status == PROC_WAITING){
			pp->status = PROC_READY;
			schedule_proc();
			enable();
			return;
		}
	}
	enable();
}

// passive blocked
// void blocked()

// wait for child process
void do_wait() {
	disable();
#ifdef DEBUG
	put_info("In wait");
#endif
	curr_proc->status = PROC_WAITING;
	schedule_proc();
	enable();
}

// RUNNING -> TERMINATED
void do_exit() {
	disable();
#ifdef DEBUG
	put_info("In exit");
#endif
	curr_proc->status = PROC_TERMINATED;
	if (curr_proc->parent != NULL)
		wakeup(curr_proc->parent->pid);
	enable();
}

int kill(uint8_t pid) {
	disable();
#ifdef DEBUG
	put_info("In kill");
#endif
	for (process* pp = &proc_list[0]; pp < &proc_list[MAX_PROCESS]; ++pp){
		if (pp->pid == pid){
			pp->status = PROC_TERMINATED;
			enable();
			return 0;
		}
	}
	enable();
	return -1;
}

#endif // TASK_H