// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** hal.h ******/

#ifndef HAL_H
#define HAL_H

#include "idt.h"
// #include "pic.h"
#include "keyboard.h"
// #include "pit.h"
// #include "dma.h"
// #include "flpydsk.h"

//! returns current tick count (only for demo)
// int get_tick_count () {

// 	return i86_pit_get_tick_count();
// }

void hal_initialize(){
	/*
	 * Global Description Table (GDT) has been initialized in switch_to_pm.asm
	 */


	/*
	 * Interrupt Description Table (IDT) initialization
	 */
	idt_init();

	/*
	 * Programmable Interrupt Controller (PIC) initialization
	 */
	pic_init();
	// pic_initialize(0x20,0x28);

	/*
	 * Keyboard initialization
	 */
	kb_init();

	// i86_pit_initialize ();
	// i86_pit_start_counter (100,I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	// flpydsk_set_working_drive(0);
	// flpydsk_init(38);
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

#endif // HAL_H