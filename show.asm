; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

; Define some constants
VIDEO_MEMORY_SEG equ 0xb800
VIDEO_MEMORY    equ 0xb8000
WHITE_ON_BLACK  equ 0x07

;;;;; Real mode output ;;;;;
print_string_rm:
; Input:
;    bx: the starting address of the string
;    dh: row
;    dl: col
    pusha
    push es
    mov cx, VIDEO_MEMORY_SEG      ; Set cx to the start of VMEM
    mov es, cx
    ; Compute memory address
    xor ax, ax                    ; ax = 0
    mov al, dh                    ; ax = row
    mov cx, 80                    ; cx = 80
    mul cx                        ; dst-op: ax = 80*row, src-op: parameter (cx)
    add al, dl                    ; ax = 80*row + col
    mov cx, 2                     ; cx = 2
    mul cx                        ; ax = (80*row + col) * 2
print_string_rm_loop:
    ; Store char ASCII and attributes
    mov cl, [ bx ]                ; Store the char at bx in cl
    mov ch, WHITE_ON_BLACK        ; Store the attributes in ch
    cmp cl, 0                     ; if (cl == 0), at end of string, so
    je print_string_rm_done       ; jump to done
    ; Show char
    mov bp, ax                    ; position
    mov [ es:bp ], cx             ; Store char and attributes at current character cell
    add bx, 1                     ; Increment bx to the next char in string
    add ax, 2                     ; Move to next character cell in VMEM
    jmp print_string_rm_loop      ; loop around to print the next char
print_string_rm_done:
    pop es
    popa
    ret                           ; Return from the function

;;;;; Protected mode output ;;;;;

[ bits 32 ]

print_string:
; Input:
;    ebx: the starting address of the string
;    dh: row
;    dl: col
    pusha
    ; Compute memory address
    xor eax, eax                  ; eax = 0
    mov al, dh                    ; eax = row
    mov ecx, 80                   ; ecx = 80
    mul ecx                       ; dst-op: eax = 80*row, src-op: parameter (ecx)
    add al, dl                    ; eax = 80*row + col
    mov ecx, 2                    ; ecx = 2
    mul ecx                       ; eax = (80*row + col) * 2
    add eax, VIDEO_MEMORY
print_string_loop:
    mov cl, [ ebx ]               ; Store the char at ebx in al
    mov ch, WHITE_ON_BLACK        ; Store the attributes in ah
    cmp cl, 0                     ; if (al == 0), at end of string, so
    je print_string_done          ; jump to done
    mov [ eax ], cx               ; Store char and attributes at current character cell
    add ebx, 1                    ; Increment ebx to the next char in string
    add eax, 2                    ; Move to next character cell in VMEM
    jmp print_string_loop      ; loop around to print the next char
print_string_done:
    popa
    ret                           ; Return from the function