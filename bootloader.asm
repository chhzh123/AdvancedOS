; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

;;;;; bootloader.asm ;;;;;
; A boot sector that enters 32-bit protected mode

;;;;; Initialization ;;;;;
	org 0x7c00

	KERNEL_OFFSET equ 0x7e00
	SECTOR_SIZE equ 0x200 ; 512B

	mov bp, 0x7c00 ; Set the stack.
	mov sp, bp
	
	mov dh, 0
	mov dl, 0
	mov bx, MSG_REAL_MODE
	call print_string_rm

	call load_kernel

	call switch_to_pm ; Note that we never return from here.
	jmp $

	%include "include/show.inc"

[ bits 16 ]

	%include "include/disk_load.inc"

load_kernel:
	show_string_rm MSG_LOAD_KERNEL, 1, 0
	load_sectors KERNEL_OFFSET, 35, 0, 0, 2
	load_sectors KERNEL_OFFSET+SECTOR_SIZE*35, 25, 1, 0, 1
	ret

;;;;; Swich from real mode to protected mode ;;;;;

[ bits 16 ]

	%include "include/gdt.inc"

switch_to_pm:
	cli                         ; We MUST switch of interrupts until we have
	                            ; set-up the protected mode interrupt vector
	                            ; otherwise interrupts will run riot.
	lgdt [ gdt_descriptor ]     ; Load our global descriptor table, which defines
	                            ; the protected mode segments ( e.g. for code and data )
	mov eax, cr0                ; To make the switch to protected mode, we set
	or eax, 0x1                 ; the first bit of CR0, a control register
	mov cr0, eax
	jmp CODE_SEG:init_pm        ; Make a far jump ( i.e. to a new segment ) to our 32-bit
								; code. This also forces the CPU to flush its cache of
								; pre - fetched and real - mode decoded instructions, which can
								; cause problems.
								; CODE_SEG is defined in gdt.asm, which is 0x08 (skip the first null descriptor)

[ bits 32 ]

; Initialise registers and the stack once in PM.
init_pm:
	mov ax, DATA_SEG            ; Now in PM, our old segments are meaningless,
	mov ds, ax                  ; so we point our segment registers to the
	mov ss, ax                  ; data selector we defined in our GDT
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ebp, 0x90000            ; Update our stack position so it is right
	mov esp, ebp                ; at the top of the free space.
	call BEGIN_PM               ; Finally, call some well - known label

;;;;; Begin execute protected mode code ;;;;;

[ bits 32 ]

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