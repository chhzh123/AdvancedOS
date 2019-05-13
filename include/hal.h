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
#include "ide.h"

void hal_initialize(){
	printf("\n\n\n");
	/*
	 * Global Descriptor Table (GDT) and task state segment (TSS) initialization
	 */
	gdt_init(); // re-implement for completeness
	put_info("Initialized GDT & TSS");

	/*
	 * Interrupt Descriptor Table (IDT) initialization
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

	put_info("Initialized IDT");

	/*
	 * Programmable Interrupt Controller (PIC) initialization
	 */
	pic_init();
	put_info("Initialized PIC");

	/*
	 * Keyboard initialization
	 */
	kb_init();
	put_info("Initialized keyboard");

	/*
	 * Programmable Interval Timer (PIT) initialization
	 */
	pit_init();
	pit_start_counter(100, PIT_OCW_COUNTER_0, PIT_OCW_MODE_SQUAREWAVEGEN);
	put_info("Initialized PIT");
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