; Copyright (c) 2019 Hongzheng Chen
; chenhzh37@mail2.sysu.edu.cn
; Ubuntu 18.04 + nasm 2.13.02

; void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
; {
;     *cyl    = lba / (2 * 18);
;     *head   = ((lba % (2 * 18)) / 18);
;     *sector = ((lba % (2 * 18)) % 18 + 1);
; }
%macro load_sectors 5
	mov bx, %1   ; Setup parameters for our disk_load routine
	mov al, %2   ; # of sectors
	mov ch, %3   ; cylinder (default: 0)
	mov dh, %4   ; head (default: 0)
	mov cl, %5   ; starting sector (default: 2)
	call disk_load
%endmacro

; load DH sectors to ES:BX from drive DL
disk_load:
	mov dl, 0x00   ; Select floppy disk
	mov ah, 0x02   ; BIOS read sector function
	int 0x13       ; BIOS interrupt
	jc disk_error  ; Jump if error ( i.e. carry flag set )
	ret

disk_error:
	show_string_rm DISK_ERROR_MSG, 0, 0
	jmp $

	DISK_ERROR_MSG db " Disk read error!", 0