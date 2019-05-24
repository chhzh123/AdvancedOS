// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** exception.h ******/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "stdio.h"

/*
 * | eflags | esp+12
 * |   cs   | esp+8
 * |   eip  | esp+4
 * |  error | esp
 */

// divide by 0 fault
__attribute__((__cdecl__))
void divide_by_zero_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Divide by 0");
	for (;;);
}

// single step
__attribute__((__cdecl__))
void single_step_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Single step");
	for (;;);
}

// non maskable trap
__attribute__((__cdecl__))
void nmi_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: NMI trap");
	for (;;);
}

// breakpoint hit
__attribute__((__cdecl__))
void breakpoint_trap (unsigned int cs,unsigned int eip, unsigned int eflags) {
	put_error("Exception: Breakpoint trap");
	for (;;);
}

// overflow
__attribute__((__cdecl__))
void overflow_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Overflow trap");
	for (;;);
}

// bounds check
__attribute__((__cdecl__))
void bounds_check_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Bounds check fault");
	for (;;);
}

/*
 * | eflags |
 * | cs     |
 * | eip    |
 * | error  | <- esp
 */ 
// invalid opcode / instruction
__attribute__((__cdecl__))
void invalid_opcode_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	char str[100];
	sprintf(str,"Exception: Invalid opcode\ncs:eip=%xh:%xh, eflags=%xh",cs,eip,eflags);
	put_error(str);
	for (;;);
}

// device not available
__attribute__((__cdecl__))
void no_device_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Device not found");
	for (;;);
}

// double fault
__attribute__((__cdecl__))
void double_fault_abort (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Double fault");
	for (;;);
}

// invalid Task State Segment (TSS)
__attribute__((__cdecl__))
void invalid_tss_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Invalid TSS");
	for (;;);
}

// segment not present
__attribute__((__cdecl__))
void no_segment_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Invalid segment");
	for (;;);
}

// stack fault
__attribute__((__cdecl__))
void stack_fault ( unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Stack fault");
	for (;;);
}

// general protection fault
__attribute__((__cdecl__))
void general_protection_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	char str[100];
	sprintf(str,"Exception: General Protection Fault\ncs:eip=%xh:%xh, eflags=%xh, err=%xh",cs,eip,eflags,err);
	put_error(str);
	for (;;);
}

// page fault
__attribute__((__cdecl__))
void page_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Page Fault");
	for (;;);
}

// Floating Point Unit (FPU) error
__attribute__((__cdecl__))
void fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: FPU Fault");
	for (;;);
}

// alignment check
__attribute__((__cdecl__))
void alignment_check_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Alignment Check");
	for (;;);
}

// machine check
__attribute__((__cdecl__))
void machine_check_abort (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: Machine Check");
	for (;;);
}

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
__attribute__((__cdecl__))
void simd_fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	put_error("Exception: FPU SIMD fault");
	for (;;);
}

#endif