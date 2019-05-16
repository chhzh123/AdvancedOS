// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** ide.h ******/

#ifndef IDE_H
#define IDE_H

#define SECTSIZE 512
#define TMP_USER_ADDR 0x1000

#include "string.h"

static inline void insw(uint32_t port, uintptr_t addr, int cnt) {
    __asm__ volatile (
    	"mov di,ax\n\t" // destination
    	"rep insw\n\t" // repeat input string
    	:
    	:"a"(addr),"d"(port),"c"(cnt)
    	);
}

/* waitdisk - wait for disk ready */
static inline void waitdisk(void) {
	while ((port_byte_in(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void readsect(uintptr_t dst, uint32_t secno) {

	disable(); // IMPORTANT!!!

    port_byte_out(0x1F2, 1);                         // count = 1
    port_byte_out(0x1F3, secno & 0xFF);
    port_byte_out(0x1F4, (secno >> 8) & 0xFF);
    port_byte_out(0x1F5, (secno >> 16) & 0xFF);
    port_byte_out(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    port_byte_out(0x1F7, 0x20);                      // cmd 0x20 - read sectors
    // wait for disk to be ready
    waitdisk();

    // read a sector
    insw(0x1F0, dst, SECTSIZE / 2);
}

// read [@startsec,@startsec+@cnt] sectors to @addr
static void read_sectors(uintptr_t addr, uint32_t startsec, uint32_t cnt)
{
	for (int i = 0; i < cnt; i++){
		readsect(TMP_USER_ADDR + i * SECTSIZE, startsec + i);
		memcpy((void*)addr, (const void*)TMP_USER_ADDR, cnt * SECTSIZE);
	}
}

void show_one_sector(uintptr_t addr)
{
	printf("Addr: %xh\n", addr);
	uint8_t* sector = (uint8_t*) addr;
	int i = 0;
	for (int c = 0; c < 4; c++ ) {
		for (int j = 0; j < 128; j++){
			printf ("%x", sector[ i + j ]);
			if (j % 2 == 1)
				printf(" ");
		}
		i += 128;
	}
	printf("\n");
}

void read_disk_test(){
	printf("Please enter the sector number: ");
	uint8_t* sector = (uint8_t*)0x1000;
	int num = 0;
	scanf("%d",&num);
	printf("Reading sector %d...\n", num);
	read_sectors((uintptr_t)sector, num, 1);
	// display sector
	printf("Address: %x\n", (uintptr_t)sector);
	show_one_sector(0x1000);
}

#endif // IDE_H