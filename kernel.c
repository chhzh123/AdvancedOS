// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** kernel.c ******/

#include "hal.h"
#include "task.h"
#include "terminal.h"

void initialize()
{
	/*
	 * Hardware Abstraction Layer (HAL) initialization
	 */
	hal_initialize();

	/*
	 * Process initialization
	 */
	proc_init();
	put_info("Initialized process");
}

void main () {
	initialize();
	clear_screen();
	// create_user_proc();
	// while(1){};
	terminal();
}