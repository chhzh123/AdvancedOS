// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** kernel.c ******/

#include "hal.h"
#include "task.h"
#include "semaphore.h"
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

	/*
	 * Semaphore initialization
	 */
	sem_init();
}

void main () {
	initialize();
	clear_screen();
	terminal();
}