; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

;;;;; bootloader.asm ;;;;;
; A boot sector that enters 32-bit protected mode

;;;;; Initialization ;;;;;
	org 0x7c00

	KERNEL_OFFSET equ 0x1000

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

	call load_kernel

	call switch_to_pm ; Note that we never return from here.
	jmp $

	%include "show.asm"
	%include "gdt.asm"
	%include "switch_to_pm.asm"
	%include "disk_load.asm"

[ bits 16 ]

load_kernel:
	show_string_rm MSG_LOAD_KERNEL, 1, 0
	mov bx, KERNEL_OFFSET  ; Setup parameters for our disk_load routine
	mov dh, 40             ; load the first n sectors
	call disk_load
	ret

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
	show_string_pm MSG_PROT_MODE, 2, 0

	call KERNEL_OFFSET ; Now jump to the address of our loaded
                       ; kernel code, assume the brace position,
                       ; and cross your fingers. Here we go!

    show_string_pm SEE_YOU_MSG, 10, 0
	jmp $ ; Hang

; Global variables
GLOBAL_VARIABLES:
	BOOT_DRIVE db 0
	MSG_REAL_MODE db "Started in 16-bit Real Mode.", 0
	MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode!", 0
	MSG_LOAD_KERNEL db "Loading kernel into memory..." , 0
	SEE_YOU_MSG db 'See you again in CHZOS next time! Byebye!', 0