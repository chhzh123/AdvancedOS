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
	mov ax, 0x10                ; kernel data selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:load_gdt_ret
load_gdt_ret:
	ret

;;;;; Show infrastructure ;;;;;

[ global show_static_string ]
%include "include/show.inc"

show_static_string:
	push ebp
	mov ebp, esp
	push ecx
	push edx
	mov ecx, [ ebp + 8 ] ; argment 1 (string address)
	mov dh, byte [ ebp + 12 ] ; argment 2 (row)
	show_string_pm ecx, dh, 0
	pop edx
	pop ecx
	pop ebp
	ret

;;;;; Interrupt ;;;;;

[ global load_idt ]
[ global keyboard_handler ]
[ global pit_handler ]
[ global sys_interrupt_handler ]
[ extern keyboard_handler_main ]
[ extern pit_handler_main ]
[ extern sys_interrupt_handler_main ]

load_idt:
	mov edx, [ esp + 4 ]
	lidt [ edx ]        ; load interrupt description table (IDT)
	sti                 ; turn on interrupts
	ret

keyboard_handler:
	cli
	call    keyboard_handler_main
	sti
	iretd               ; 32-bit return

pit_handler:
	cli
	call pit_handler_main
	sti
	iretd

sys_interrupt_handler:
	cli
	call sys_interrupt_handler_main
	sti
	iretd

;;;;; user mode ;;;;;

[ global enter_usermode ]

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

	pushf			; EFLAGS, remember to set up INTERRUPTS!!!
	pop eax         ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop
	or eax, 0x200   ; Set the IF flag
	push eax        ; Push the new EFLAGS value back onto the stack

	push 0x1b		; CS, user mode code selector is 0x18 (GDT 3). With RPL 3 is 0x1b
	push ecx        ; EIP
	iret


;;;;; Process ;;;;;

; [ extern curr_proc ]
[ global save_proc_entry ]
[ extern save_proc ]
[ global restart_proc ]

save_proc_entry:
	pusha ; ax,cx,dx,bx,sp,bp,si,di
	push ds
	push es
	push fs
	push gs

	push ax
	mov ax, 0x10
	mov ds, ax
	pop ax

	push esp
	call save_proc
	pop esp

	pop gs
	pop fs
	pop es
	pop ds
	popa
	ret

restart_proc:
	cli
	add esp, 4
	pop eax      ; ds
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov eax, esp
	add eax, 20
	mov [ esp + 12 ], eax
	; jmp $

	; | ss     | ; esp+16
	; | esp    | ; esp+12
	; | eflags | ; esp+8
	; | cs     | ; esp+4
	; | eip    | ; esp
	iretd ; flush cs:eip

;;;;;
; The DIV instruction (and it's counterpart IDIV for signed numbers)
; gives both the quotient and remainder (modulo).
; DIV r16 divides a 32-bit number in DX:AX by a 16-bit operand and
; stores the quotient in AX and the remainder in DX.
; e.g.
; mov dx, 0
; mov ax, 1234
; mov bx, 10
; div bx       ; Divides 1234 by 10. DX = 4 and AX = 123
;;;;;
; int_to_str: ; reverse!
; 	; ax: input number
; 	mov ecx, numstr
; int_to_str_loop:
; 	mov ebx, 10
; 	div ebx
; 	add dl, '0'
; 	mov byte [ ecx ], dl
; 	inc ecx
; 	cmp eax, 0
; 	jne int_to_str_loop
; 	mov byte [ ecx ], 0
; 	show_string_pm msg, 15, 0
; 	jmp $
; 	ret

testdata:
	msg db "This is a test message!", 0
; 	numstr db "                ", 0