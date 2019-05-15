// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** pit.h ******/

#ifndef PIT_H
#define PIT_H

#include "idt.h"
#include "pic.h"
#include "stdio.h"
#include <stdint.h>

// Operation Command Word (OCW)

#define		PIT_OCW_MASK_BINCOUNT		1		//00000001
#define		PIT_OCW_MASK_MODE			0xE		//00001110
#define		PIT_OCW_MASK_RL				0x30	//00110000
#define		PIT_OCW_MASK_COUNTER		0xC0	//11000000

// Use when setting binary count mode
#define		PIT_OCW_BINCOUNT_BINARY		0		//0
#define		PIT_OCW_BINCOUNT_BCD		1		//1

// Use when setting counter mode
#define		PIT_OCW_MODE_TERMINALCOUNT	0		//0000
#define		PIT_OCW_MODE_ONESHOT		0x2		//0010
#define		PIT_OCW_MODE_RATEGEN		0x4		//0100
#define		PIT_OCW_MODE_SQUAREWAVEGEN	0x6		//0110
#define		PIT_OCW_MODE_SOFTWARETRIG	0x8		//1000
#define		PIT_OCW_MODE_HARDWARETRIG	0xA		//1010

// Use when setting data transfer
#define		PIT_OCW_RL_LATCH			0			//000000
#define		PIT_OCW_RL_LSBONLY			0x10		//010000
#define		PIT_OCW_RL_MSBONLY			0x20		//100000
#define		PIT_OCW_RL_DATA				0x30		//110000

// Use when setting the counter we are working with
#define		PIT_OCW_COUNTER_0			0		//00000000
#define		PIT_OCW_COUNTER_1			0x40	//01000000
#define		PIT_OCW_COUNTER_2			0x80	//10000000

#define		PIT_REG_COUNTER0		0x40
#define		PIT_REG_COUNTER1		0x41
#define		PIT_REG_COUNTER2		0x42
#define		PIT_REG_COMMAND			0x43

// Global Tick count
static volatile uint32_t			pit_ticks = 0;

// Test if pit is initialized
static bool							pit_bIsInit = false;

// Sets new pit tick count and returns prev. value
uint32_t pit_set_tick_count (uint32_t i) {

	uint32_t ret = pit_ticks;
	pit_ticks = i;
	return ret;
}

// returns current tick count
uint32_t pit_get_tick_count () {

	return pit_ticks;
}

int get_tick_count () {

	return pit_get_tick_count();
}

// send command to pic
void pit_send_command (uint8_t cmd) {

	port_byte_out (PIT_REG_COMMAND, cmd);
}

// send data to a counter
void pit_send_data (uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter == PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

	port_byte_out (port, (uint8_t)data);
}

// read data from counter
uint8_t pit_read_data (uint16_t counter) {

	uint8_t	port = (counter == PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter == PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

	return port_byte_in (port);
}

// starts a counter
void pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode) {

	if (freq == 0)
		return;

	uint16_t divisor = (uint16_t)(1193181 / (uint16_t)freq);

	// send operational command
	uint8_t ocw = 0;
	ocw = (ocw & ~PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
	ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | counter;
	pit_send_command (ocw);

	// set frequency rate
	pit_send_data (divisor & 0xff, 0);
	pit_send_data ((divisor >> 8) & 0xff, 0);

	// reset tick count
	pit_ticks = 0;
}

extern void pit_handler();

// initialize minidriver
void pit_init() {

	setvect (32, (unsigned long)pit_handler); // pit uses 32 interrupt

	// initialized
	pit_bIsInit = true;
}

// test if pit interface is initialized
bool pit_is_initialized () {

	return pit_bIsInit;
}

#endif // PIT_H