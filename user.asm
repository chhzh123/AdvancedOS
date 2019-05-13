; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

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