; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn

; This program uses to test the system call

	[ bits 32 ]

	org 70000h

	jmp main

%include "include/show.inc"

main:
	mov eax, 0
	int 80h
	mov eax, 1
	int 80h
	jmp $

data:
	msg db "In user program sys_test!", 0