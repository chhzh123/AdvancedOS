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

/* global segment number */
#define SEG_KTEXT   1
#define SEG_KDATA   2
#define SEG_UTEXT   3
#define SEG_UDATA   4
#define SEG_TSS     5

/* global descrptor numbers */
#define GD_KTEXT    ((SEG_KTEXT) << 3)      // kernel text
#define GD_KDATA    ((SEG_KDATA) << 3)      // kernel data
#define GD_UTEXT    ((SEG_UTEXT) << 3)      // user text
#define GD_UDATA    ((SEG_UDATA) << 3)      // user data
#define GD_TSS      ((SEG_TSS) << 3)        // task segment selector

/*
 * CPL: Current Privilege Level
 * RPL: Request Privilege Level
 * DPL: Descriptor Privilege Level
 */
#define DPL_KERNEL  (0)
#define DPL_USER    (3)

#define KERNEL_CS   ((GD_KTEXT) | DPL_KERNEL) // 0x08
#define KERNEL_DS   ((GD_KDATA) | DPL_KERNEL) // 0x10
#define USER_CS     ((GD_UTEXT) | DPL_USER)   // 0x18 | 3
#define USER_DS     ((GD_UDATA) | DPL_USER)   // 0x20 | 3

#include "tss.h"

// gdt descriptor. A gdt descriptor defines the properties of a specific
// memory block and permissions.

struct gdt_descriptor {

	// bits 0-15 of segment limit
	uint16_t		limit;

	// bits 0-23 of base address
	uint16_t		baseLo;
	uint8_t			baseMid;

	/*
	 * descriptor access flags
	 * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	 * | P |  DPL  | S |      TYPE     |
	 * P: Present in memory or not
	 * DPL(Descriptor Privilege Level): ring 0 - ring 3
	 * S: 1 - Data/Code descriptor | 0 - gate descriptor
	 * Type: When S = 1
	 *    3: Executable
	 *    2: Consistent
	 *    1: 1 - Read+Write | 0 - Only read
	 *    0: Accessed
	 */
	uint8_t			flags; // access

	/*
	 * grand
	 * | 7 |  6  | 5 |  4  | 3 | 2 | 1 | 0 |
	 * | G | D/B | 0 | AVL |   Limit high  |
	 * G: 0 - B | 1 - 4KB
	 */
	uint8_t			grand; // limit_high, flags

	// bits 24-32 of base address
	uint8_t			baseHi;
} __attribute__((packed));

// processor gdtr register points to base of gdt. This helps
// us set up the pointer
struct gdtr {

	// size of gdt
	uint16_t		m_limit;

	// base address of gdt
	uint32_t		m_base;
} __attribute__((packed));

// global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt [MAX_DESCRIPTORS];

// gdtr data
struct gdtr				_gdtr; // used for assembly

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

void tss_init(uint32_t sel) {
	uint32_t base = (uint32_t) &tss;
	gdt_set_descriptor (sel, base, base + sizeof(tss),
		GDT_DESC_ACCESS|GDT_DESC_EXEC_CODE|GDT_DESC_DPL|GDT_DESC_MEMORY,
		0);
}

// install gdtr
extern void load_gdt(); // assembly

// initialize gdt
int gdt_init () {

	// set up gdtr
	_gdtr.m_limit = (sizeof (struct gdt_descriptor) * MAX_DESCRIPTORS)-1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	// set null descriptor (0x00)
	gdt_set_descriptor(0, 0, 0, 0, 0);

	// set default code descriptor (0x08)
	gdt_set_descriptor (1,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_EXEC_CODE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default data descriptor (0x10)
	gdt_set_descriptor (2,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default user mode code descriptor (0x18)
	gdt_set_descriptor (3,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_EXEC_CODE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY|GDT_DESC_DPL,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	// set default user mode data descriptor (0x20)
	gdt_set_descriptor (4,0,0xffffffff,
		GDT_DESC_READWRITE|GDT_DESC_CODEDATA|GDT_DESC_MEMORY|GDT_DESC_DPL,
		GDT_GRAND_4K | GDT_GRAND_32BIT | GDT_GRAND_LIMITHI_MASK);

	tss_init(5);
	load_gdt(); // assembly
	tss_install(5);

	return 0;
}

#endif // GDT_H