; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

;;;;; bootloader.asm ;;;;;
; A boot sector that enters 32-bit protected mode

;;;;; Initialization ;;;;;
	org 0x7c00

%macro show_string_rm 3
	; msg, row, col
	mov dh, %2
	mov dl, %3
	mov bx, %1
	call print_string_rm
%endmacro

	mov bp, 0x9000 ; Set the stack.
	mov sp, bp
	
	show_string_rm MSG_REAL_MODE, 0, 0
	call switch_to_pm ; Note that we never return from here.
	jmp $

	%include "show.asm"
	%include "gdt.asm"
	%include "switch_to_pm.asm"

[ bits 32 ]

%macro show_string_pm 3
	; msg, row, col
	mov dh, %2
	mov dl, %3
	mov bx, %1
	call print_string
%endmacro

; This is where we arrive after switching to and initialising protected mode
BEGIN_PM:
	mov ebx, MSG_PROT_MODE
	show_string_pm MSG_PROT_MODE, 1, 0
	jmp $ ; Hang

; Global variables
GLOBAL_VARIABLES:
	MSG_REAL_MODE db "Started in 16-bit Real Mode.", 0
	MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode!", 0