// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** kernel.c ******/
#include "stdio.h"
#include "terminal.h"

void main () {
	idt_init();
	clear_screen();
	terminal();
}