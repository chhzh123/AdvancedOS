// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** kernel.c ******/

#include "hal.h"
#include "terminal.h"

void main () {
	hal_initialize();
	clear_screen();
	generate_interrupt(1);
	// printf("%d\n",get_tick_count());
	terminal();
}