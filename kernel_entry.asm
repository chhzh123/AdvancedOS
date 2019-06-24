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
[ global sys_pthread_handler ]
[ global sys_file_handler ]
[ extern keyboard_handler_main ]
[ extern pit_handler_main ]
[ extern sys_interrupt_handler_main ]
[ extern sys_pthread_handler_main ]
[ extern sys_file_handler_main ]

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

pit_handler:            ; handle process switching
	cli
	jmp pit_handler_entry ; DO NOT USE CALL!!! WILL DESTROY STACK
pit_handler_entry_ret:
	sti
	iretd

sys_interrupt_handler:
	cli
	jmp save_proc_entry
save_proc_entry_ret:
	push eax            ; Functional number
	call sys_interrupt_handler_main
	pop ebx             ; Remember to pop out
	; eax store the return value
	sti
	iretd

sys_pthread_handler:
	cli
	pusha ; ax,cx,dx,bx,sp,bp,si,di
	push ds
	push es
	push fs
	push gs

	call save_proc

	pop gs
	pop fs
	pop es
	pop ds
	popa

	push eax            ; Functional number
	call sys_pthread_handler_main
	pop ebx             ; Remember to pop out
	; eax store the return value
	sti
	iretd

sys_file_handler:
	cli
	pusha ; ax,cx,dx,bx,sp,bp,si,di
	push ds
	push es
	push fs
	push gs

	call save_proc

	pop gs
	pop fs
	pop es
	pop ds
	popa

	push eax            ; Functional number
	call sys_file_handler_main
	pop ebx             ; Remember to pop out
	; eax store the return value
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

[ global pit_handler_entry ]
[ extern save_proc ]
[ global restart_proc ]

;;;;;
; From Intel manual p2880:
; If the handler procedure is going to be executed
; at the same privilege level as the interrupted procedure:
; a. The processor saves the current state of the
;    EFLAGS, CS, and EIP registers on the current stack
; b. If an exception causes an error code to be saved,
;    it is pushed on the current stack after the EIP value
;
; If the handler procedure is going to be executed at a
; numerically lower privilege level, a stack switch occurs.
; When the stack switch occurs:
; a. The segment selector and stack pointer for the stack
;    to be used by the handler are obtained from the TSS
;    for the currently executing task.
;    On this new stack, the processor pushes
;    the stack segment selector and stack pointer
;    of the interrupted procedure.
; b. The processor then saves the current state of the
;    EFLAGS, CS, and EIP registers on the new stack
; c. If an exception causes an error code to be saved,
;    it is pushed on the new stack after the EIP value.
;;;;;
; STACK CHANGE! (different privilege level)
; | ss     |
; | esp    | ; user stack esp!!!
; NO STACK CHANGE! (the same privilege level)
; | eflags |
; | cs     |
; | eip    |
; NO ERROR CODE!
; | ax     |
; | cx     |
; | dx     |
; | bx     |
; | sp     | ; kernel stack esp
; | bp     |
; | si     |
; | di     |
; | ds     |
; | es     |
; | fs     |
; | gs     |
; | ret-add| ; pit_handler_main
pit_handler_entry:
	pusha ; ax,cx,dx,bx,sp,bp,si,di
	push ds
	push es
	push fs
	push gs

	call pit_handler_main

	pop gs
	pop fs
	pop es
	pop ds
	popa
	jmp pit_handler_entry_ret

save_proc_entry:
	pusha ; ax,cx,dx,bx,sp,bp,si,di
	push ds
	push es
	push fs
	push gs

	call save_proc

	pop gs
	pop fs
	pop es
	pop ds
	popa
	jmp save_proc_entry_ret

; | ss     | ; esp+16
; | esp    | ; esp+12
; | eflags | ; esp+8
; | cs     | ; esp+4
; | eip    | ; esp (after `popa`)
; | eax    |
; | ecx    |
; | edx    |
; | ebx    |
; | esp    |
; | ebp    |
; | esi    |
; | edi    |
; | ds     |
; | es     |
; | fs     |
; | gs     | <- now esp
; | ret-add|
restart_proc:
	cli
	add esp, 4   ; skip return address
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iretd ; flush cs:eip

;;;;; read process info ;;;;;

[ global read_eip ]

read_eip:
	pop eax ; get return address
	jmp eax ; return by jmp

testdata:
	msg db "This is a test message!", 0