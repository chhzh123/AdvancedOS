; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

; Ensures that we jump straight into the kernel ’s entry function.
[ bits 32 ]     ; We ’re in protected mode by now, so use 32-bit instructions
[ extern main ] ; Declate that we will be referencing the external symbol 'main',
                ; so the linker can substitute the final address
[ global _start ]

_start:
	call main   ; invoke main() in C kernel
	jmp $       ; Hang forever when we return from the kernel

%include "interrupt.asm"
%include "user.asm"

[ global load_gdt ]
[ extern _gdtr ]

load_gdt:
	lgdt [ _gdtr ]
	mov ax, 0x10                ; kernel mode
	mov ds, ax                  ; so we point our segment registers to the
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax                  ; data selector we defined in our GDT
	jmp 0x08:load_gdt_ret
load_gdt_ret:
	ret