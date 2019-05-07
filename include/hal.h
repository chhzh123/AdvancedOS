// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** hal.h ******/

#ifndef HAL_H
#define HAL_H

#include "gdt.h"
#include "idt.h"
#include "exception.h"
#include "pic.h"
#include "keyboard.h"
#include "pit.h"
#include "flpydsk.h"
// #include "tss.h"

void hal_initialize(){
	/*
	 * Global Description Table (GDT) initialization
	 */
	gdt_init(); // re-implement for completeness

	/*
	 * Interrupt Description Table (IDT) initialization
	 */
	idt_init();

	// install exception handlers
	setvect (0, (unsigned long) divide_by_zero_fault);
	setvect (1, (unsigned long) single_step_trap);
	setvect (2, (unsigned long) nmi_trap);
	setvect (3, (unsigned long) breakpoint_trap);
	setvect (4, (unsigned long) overflow_trap);
	setvect (5, (unsigned long) bounds_check_fault);
	setvect (6, (unsigned long) invalid_opcode_fault);
	setvect (7, (unsigned long) no_device_fault);
	setvect (8, (unsigned long) double_fault_abort);
	setvect (10, (unsigned long) invalid_tss_fault);
	setvect (11, (unsigned long) no_segment_fault);
	setvect (12, (unsigned long) stack_fault);
	setvect (13, (unsigned long) general_protection_fault);
	setvect (14, (unsigned long) page_fault);
	setvect (16, (unsigned long) fpu_fault);
	setvect (17, (unsigned long) alignment_check_fault);
	setvect (18, (unsigned long) machine_check_abort);
	setvect (19, (unsigned long) simd_fpu_fault);

	/*
	 * Programmable Interrupt Controller (PIC) initialization
	 */
	pic_init();

	/*
	 * Keyboard initialization
	 */
	kb_init();
	// port_byte_out(0x21, 0xFD); // 1101
	// port_byte_out(0xA1, 0xFD);

	/*
	 * Programmable Interval Timer (PIT) initialization
	 */
	pit_init();
	pit_start_counter(100, PIT_OCW_COUNTER_0, PIT_OCW_MODE_SQUAREWAVEGEN);

	/*
	 * floppy disk initialization
	 */
	flpydsk_set_working_drive(0);
	flpydsk_init(38);

	/*
	 * task state segment (TSS) initialization
	 */
	// tss_init();
}

void generate_interrupt(int n){
	__asm__ (
			"mov byte ptr [genint+1], al\n\t"
			"jmp genint\n"
			"genint:\n\t"
			"int 0\n\t" // above code modifies the 0 to int number to generate
			:
			:"a"(n)
			);
}

void sleep (int ms) {
	int ticks = ms + get_tick_count ();
	while (ticks > get_tick_count ());
}

#endif // HAL_H