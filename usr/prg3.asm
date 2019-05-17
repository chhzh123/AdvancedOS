; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn

; Graphic memory: 25*80 Text Mode

[ bits 32 ]

; Constants
Dn_Rt equ 1     ; D-Down, U-Up, R-right, L-Left
Up_Rt equ 2
Up_Lt equ 3
Dn_Lt equ 4
MAX_X equ 40
MAX_Y equ 25
MIN_X equ 0
MIN_Y equ 12
msgX equ 10
msgY equ 5
in_delay equ 60000 ; control the speed
out_delay equ 6000  ; outer loop

; Disk Initialization
;;; ***** REMEMBER TO MODIFY ***** ;;;
	org 40000h              ; ORG (origin) is used to set the assembler location counter

	jmp mainloop

%include "include/show.inc"

%macro OneChar 5
	; px: position x
	; py: position y
	; drt: direction
	; char: char ascii
	; color
	mov ax, word [ %1 ]
	mov word [ px ], ax
	mov ax, word [ %2 ]
	mov word [ py ], ax
	mov al, byte [ %3 ]
	mov byte [ drt ], al
	mov al, byte [ %4 ]
	mov byte [ char ], al
	mov al, byte [ %5 ]
	inc al
	cmp al, 1000b
	jl changecolor
	mov al, 00000001b
changecolor:
	mov [ color ], al         ; change colors

	call onemove

	;;; Store back to memory ;;;
	mov dx, word [ px ]
	mov word [ %1 ], dx
	mov dx, word [ py ]
	mov word [ %2 ], dx
	mov dl, byte [ drt ]           ; bytes, be careful!
	mov byte [ %3 ], dl
	mov dl, byte [ color ]
	mov byte [ %5 ], dl
%endmacro

mainloop:
	call delayloop
	OneChar px, py, drt, char, color
	show_string_color msg, msgY, msgX, 74h
	mov ax, word [ cnt ]
	dec ax
	mov word [ cnt ], ax
	cmp ax, 0
	jne mainloop
	ret

delayloop:
	mov ecx, out_delay
	outloop:
		mov eax, in_delay
		inloop:
			dec eax
			jg inloop
	loop outloop
	ret

onemove:
	cmp byte [ drt ], 1
	jz DnRt
	cmp byte [ drt ], 2
	jz UpRt
	cmp byte [ drt ], 3
	jz UpLt
	cmp byte [ drt ], 4
	jz DnLt
onemoveret:
	ret

;;;;; Down Right ;;;;;
DnRt:
	inc word [ px ]
	inc word [ py ]
	mov ax, word [ py ]
	cmp ax, MAX_Y            ; px = MAX_Y?
	je dr2ur                 ; jump if the result of the last arithmetic operation was zero
	mov ax, word [ px ]         ; PX = MAX_X?
	cmp ax, MAX_X
	je dr2dl
	jmp show
dr2ur:
	mov word [ py ], MAX_Y-2
	mov byte [ drt ], Up_Rt
	jmp show
dr2dl:
	mov word [ px ], MAX_X-2
	mov byte [ drt ], Dn_Lt
	jmp show

;;;;; Up Right ;;;;;
UpRt:
	inc word [ px ]
	dec word [ py ]
	mov ax, word [ px ]
	cmp ax, MAX_X
	je ur2ul
	mov ax, word [ py ]
	cmp ax, MIN_Y-1
	je ur2dr
	jmp show
ur2ul:
	mov word [ px ], MAX_X-2
	mov byte [ drt ], Up_Lt
	jmp show
ur2dr:
	mov word [ py ], MIN_Y+1
	mov byte [ drt ], Dn_Rt
	jmp show

;;;;; Up Left ;;;;;
UpLt:
	dec word [ px ]
	dec word [ py ]
	mov ax, word [ py ]
	cmp ax, MIN_Y-1
	je ul2dl
	mov ax, word [ px ]
	cmp ax, MIN_X-1
	je ul2ur
	jmp show
ul2dl:
	mov word [ py ], MIN_Y+1
	mov byte [ drt ], Dn_Lt
	jmp show
ul2ur:
	mov word [ px ], MIN_X+1
	mov byte [ drt ], Up_Rt
	jmp show

;;;;; Down Left ;;;;;
DnLt:
	dec word [ px ]
	inc word [ py ]
	mov ax, word [ px ]
	cmp ax, MIN_X-1
	je dl2dr
	mov ax, word [ py ]
	cmp ax, MAX_Y
	je dl2ul
	jmp show
dl2dr:
	mov word [ px ], MIN_X+1
	mov byte [ drt ], Dn_Rt
	jmp show
dl2ul:
	mov word [ py ], MAX_Y-2
	mov byte [ drt ], Up_Lt
	jmp show

;;;;; Show words ;;;;;
show:
	show_string_color char, [ py ], [ px ], [ color ]
	jmp onemoveret

;;;;; Data Segment ;;;;;
datadef:
	px dw 1
	py dw 13
	drt db Dn_Rt                    ; Down Right
	char db 'OS', 0
	charlen equ ($-char)
	color db 00000111b

	msg db 'This is prg3!', 0
	msglen equ ($-msg)

	cnt dw 1000