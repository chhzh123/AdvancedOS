// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** gdt.h ******/

#ifndef GDT_H
#define GDT_H

#include "string.h"

// maximum amount of descriptors allowed
#define MAX_DESCRIPTORS					6

/***	 gdt descriptor access bit flags.	***/

// set access bit
#define GDT_DESC_ACCESS			0x0001			//00000001
// descriptor is readable and writable. default: read only
#define GDT_DESC_READWRITE			0x0002			//00000010
// set expansion direction bit
#define GDT_DESC_EXPANSION			0x0004			//00000100
// executable code segment. Default: data segment
#define GDT_DESC_EXEC_CODE			0x0008			//00001000
// set code or data descriptor. Defult: system defined descriptor
#define GDT_DESC_CODEDATA			0x0010			//00010000
// set dpl bits
#define GDT_DESC_DPL			0x0060			//01100000
// set "in memory" bit
#define GDT_DESC_MEMORY			0x0080			//10000000

/**	gdt descriptor grandularity bit flags	***/

// masks out limitHi (High 4 bits of limit)
#define GDT_GRAND_LIMITHI_MASK		0x0f			//00001111
// set os defined bit
#define GDT_GRAND_OS			0x10			//00010000
// set if 32bit. default: 16 bit
#define GDT_GRAND_32BIT			0x40			//01000000
// 4k grandularity. default: none
#define GDT_GRAND_4K			0x80			//10000000

// gdt descriptor. A gdt descriptor defines the properties of a specific
// memory block and permissions.

struct gdt_descriptor {

	// bits 0-15 of segment limit
	uint16_t		limit;

	// bits 0-23 of base address
	uint16_t		baseLo;
	uint8_t			baseMid;

	// descriptor access flags
	uint8_t			flags;

	uint8_t			grand;

	// bits 24-32 of base address
	uint8_t			baseHi;
};

// processor gdtr register points to base of gdt. This helps
// us set up the pointer
struct gdtr {

	// size of gdt
	uint16_t		m_limit;

	// base address of gdt
	uint32_t		m_base;
};

// global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt [MAX_DESCRIPTORS];

// gdtr data
static struct gdtr				_gdtr;

// install gdtr
extern void load_gdt(unsigned long *gdt_ptr);

// Setup a descriptor in the Global Descriptor Table
void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS)
		return;

	// null out the descriptor
	memset ((void*)&_gdt[i], 0, sizeof (struct gdt_descriptor));

	// set limit and base addresses
	_gdt[i].baseLo	= (uint16_t)(base & 0xffff);
	_gdt[i].baseMid	= (uint8_t)((base >> 16) & 0xff);
	_gdt[i].baseHi	= (uint8_t)((base >> 24) & 0xff);
	_gdt[i].limit	= (uint16_t)(limit & 0xffff);

	// set flags and grandularity bytes
	_gdt[i].flags = access;
	_gdt[i].grand = (uint8_t)((limit >> 16) & 0x0f);
	_gdt[i].grand |= grand & 0xf0;
}


// returns descriptor in gdt
struct gdt_descriptor* gdt_get_descriptor (int i) {

	if (i > MAX_DESCRIPTORS)
		return 0;

	return &_gdt[i];
}

// initialize gdt
int gdt_init () {

	// set up gdtr
	_gdtr.m_limit = (sizeof (struct gdt_descriptor) * MAX_DESCRIPTORS)-1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	// set null descriptor
	gdt_set_descriptor(0, 0, 0, 0, 0);

	// set default code descriptor
	gdt_set_descriptor (1,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_EXEC_CODE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default data descriptor
	gdt_set_descriptor (2,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default user mode code descriptor
	gdt_set_descriptor (3,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_EXEC_CODE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY|GDT_DESC_DPL,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default user mode data descriptor
	gdt_set_descriptor (4,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY|GDT_DESC_DPL,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// install gdtr
	load_gdt((unsigned long*)&_gdtr); // assembly

	return 0;
}

#endif // GDT_H