; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

	[ bits 32 ]

    MAX_X equ 79
    MAX_Y equ 24
    MIN_X equ 0
    MIN_Y equ 0
    DEFAULT_COLOR equ 0007h

    in_delay equ 6000 ; control the speed
    out_delay equ 6000  ; outer loop

	org 60000h              ; ORG (origin) is used to set the assembler location counter

    jmp start

%include "include/show.inc"

start:
    call delayloop
draw_box:
    push ax
    push bx
    push cx
    push dx
    push bp
    show_string_color char, [ posy ], [ posx ], [ color ]
showchar_finished:
    mov ah, byte [posx]
    mov al, byte [posy]
    mov cl, byte [char]
    mov dl, byte [drt]
    cmp dl, 1
    je deal_lr
    cmp dl, 2
    je deal_ud
    cmp dl, 3
    je deal_rl
    cmp dl, 4
    je deal_du
draw_box_finished:
    mov bl, byte [color]
    inc bl ; change color
    mov bh, 08h
    cmp bl, bh
    jne no_change_back_color
    mov bl, 01h
no_change_back_color:
    mov byte [color], bl
    mov byte [drt], dl
    mov byte [char], cl
    mov byte [posy], al
    mov byte [posx], ah
    pop bp
    pop dx
    pop cx
    pop bx
    pop ax
    jmp start
    ret

deal_lr:
    inc ah
    cmp ah, MAX_X
    jne draw_box_finished
    mov dl, 2 ; change direction
    jmp draw_box_finished
deal_ud:
    inc al
    cmp al, MAX_Y
    jne draw_box_finished
    mov dl, 3
    jmp draw_box_finished
deal_rl:
    dec ah
    cmp ah, MIN_X
    jne draw_box_finished
    mov dl, 4
    jmp draw_box_finished
deal_du:
    dec al
    cmp al, MIN_Y
    jne draw_box_finished
    mov dl, 1
    inc cl                   ; change char
    cmp cl, 'Z'
    jne draw_box_finished
    mov cl, 'A'
    jmp draw_box_finished

delayloop:
    mov ecx, out_delay
    outloop:
        mov eax, in_delay
        inloop:
            dec eax
            jg inloop
    loop outloop
    ret

end:
	jmp $

datadef:
    posx dw 0
    posy dw 0
    char db 'A', 0
    color db 07h
    drt db 1                    ; direction: 1 lr, 2 ud, 3 rl, 4 lu