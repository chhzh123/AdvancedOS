; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

[ global load_idt ]
[ global keyboard_handler ]
[ global pit_handler ]
[ global flpydsk_handler ]
[ extern keyboard_handler_main ]
[ extern pit_handler_main ]
[ extern flpydsk_handler_main ]

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

flpydsk_handler:
	call flpydsk_handler_main
	iretd