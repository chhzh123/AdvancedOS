; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

; Ensures that we jump straight into the kernel ’s entry function.
[ bits 32 ]     ; We ’re in protected mode by now, so use 32-bit instructions
[ extern main ] ; Declate that we will be referencing the external symbol 'main',
                ; so the linker can substitute the final address
[ global _start ]
[ global load_gdt ]

_start:
	call main   ; invoke main() in C kernel
	jmp $       ; Hang forever when we return from the kernel

%include "interrupt.asm"
; %include "user.asm"

load_gdt:
	mov edx, [ esp + 4 ]
	lidt [ edx ]        ; load global description table (GDT)
	ret