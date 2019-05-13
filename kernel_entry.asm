; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

; Ensures that we jump straight into the kernel ’s entry function.
[ bits 32 ]     ; We ’re in protected mode by now, so use 32-bit instructions

;;;;;; main entry ;;;;;

[ extern main ] ; Declate that we will be referencing the external symbol 'main',
                ; so the linker can substitute the final address
[ global _start ]

_start:
	call main   ; invoke main() in C kernel
	jmp $       ; Hang forever when we return from the kernel

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

;;;;; Interrupt ;;;;;

[ global load_idt ]
[ global keyboard_handler ]
[ global pit_handler ]
[ extern keyboard_handler_main ]
[ extern pit_handler_main ]

load_idt:
	mov edx, [ esp + 4 ]
	lidt [ edx ]        ; load interrupt description table (IDT)
	sti                 ; turn on interrupts
	ret

keyboard_handler:
	call    keyboard_handler_main
	iretd               ; 32-bit return

pit_handler:
	call pit_handler_main
	iretd

;;;;; user mode ;;;;;

[ global enter_usermode ]
[ extern test_user_function ]

; | ss     | ; esp+16: the stack segment selector we want for user mode
; | esp    | ; esp+12: the user mode stack pointer
; | eflags | ; esp+8: the control flags we want to use in user mode
; | cs     | ; esp+4: the code segment selector
; | eip    | ; esp: the instruction pointer of user mode code to execute
enter_usermode:
	cli
	push ebp
	mov ebp, esp
	mov ecx, [ebp + 8] ; user program address

	mov ax, 0x23	; user mode data selector is 0x20 (GDT entry 4). Also sets RPL to 3 (user mode)
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov eax, esp
	push 0x23		; SS, notice it uses same selector as above
	push eax		; ESP
	pushf			; EFLAGS

	push 0x1b		; CS, user mode code selector is 0x18 (GDT 3). With RPL 3 is 0x1b
	push ecx ; EIP
	iret