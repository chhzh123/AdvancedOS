// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** pic.h ******/

#ifndef PIC_H
#define PIC_H

#include "io.h"
#include <stdint.h>

// The following devices use PIC 1 to generate interrupts
#define		PIC_IRQ_TIMER			0 // IMPORTANT!
#define		PIC_IRQ_KEYBOARD		1 // IMPORTANT!
#define		PIC_IRQ_SERIAL2			3
#define		PIC_IRQ_SERIAL1			4
#define		PIC_IRQ_PARALLEL2		5
#define		PIC_IRQ_DISKETTE		6 // IMPORTANT!
#define		PIC_IRQ_PARALLEL1		7

// The following devices use PIC 2 to generate interrupts
#define		PIC_IRQ_CMOSTIMER		0
#define		PIC_IRQ_CGARETRACE		1
#define		PIC_IRQ_AUXILIARY		4
#define		PIC_IRQ_FPU				5
#define		PIC_IRQ_HDC				6

//-----------------------------------------------
//	Opeation Command Word (OCW)
//-----------------------------------------------

// Command Word 2 bit masks. Use when sending commands
#define		PIC_OCW2_MASK_L1		1		// 00000001
#define		PIC_OCW2_MASK_L2		2		// 00000010
#define		PIC_OCW2_MASK_L3		4		// 00000100
#define		PIC_OCW2_MASK_EOI		0x20	// 00100000
#define		PIC_OCW2_MASK_SL		0x40	// 01000000
#define		PIC_OCW2_MASK_ROTATE	0x80	// 10000000

// Command Word 3 bit masks. Use when sending commands
#define		PIC_OCW3_MASK_RIS		1		// 00000001
#define		PIC_OCW3_MASK_RIR		2		// 00000010
#define		PIC_OCW3_MASK_MODE		4		// 00000100
#define		PIC_OCW3_MASK_SMM		0x20	// 00100000
#define		PIC_OCW3_MASK_ESMM		0x40	// 01000000
#define		PIC_OCW3_MASK_D7		0x80	// 10000000

/*
 We send commands to the command register,
 and read data from the data register.
 If we are writing from a data register,
 we are accessing the Interrupt Mask Register (IMR)
 which can be used to manually mask off or unmask interrupt requests.
 This is how we enable or disable interrupt requests.
*/
// PIC 1 register port addresses
#define PIC1_REG_COMMAND	0x20			// command register
#define PIC1_REG_STATUS		0x20			// status register
#define PIC1_REG_DATA		0x21			// data register
#define PIC1_REG_IMR		0x21			// interrupt mask register (imr)

// PIC 2 register port addresses
#define PIC2_REG_COMMAND	0xA0			// command register
#define PIC2_REG_STATUS		0xA0			// status register
#define PIC2_REG_DATA		0xA1			// data register
#define PIC2_REG_IMR		0xA1			// interrupt mask register (imr)

//-----------------------------------------------
//	Initialization Command Word (ICW)
//-----------------------------------------------

// Initialization Control Word 1 bit masks
#define PIC_ICW1_MASK_IC4			0x1			//00000001
#define PIC_ICW1_MASK_SNGL			0x2			//00000010
#define PIC_ICW1_MASK_ADI			0x4			//00000100
#define PIC_ICW1_MASK_LTIM			0x8			//00001000
#define PIC_ICW1_MASK_INIT			0x10		//00010000

// Initialization Control Words 2 and 3 do not require bit masks

// Initialization Control Word 4 bit masks
#define PIC_ICW4_MASK_UPM			0x1			//00000001
#define PIC_ICW4_MASK_AEOI			0x2			//00000010
#define PIC_ICW4_MASK_MS			0x4			//00000100
#define PIC_ICW4_MASK_BUF			0x8			//00001000
#define PIC_ICW4_MASK_SFNM			0x10		//00010000

//-----------------------------------------------
//	Initialization Command 1 control bits
//-----------------------------------------------

#define PIC_ICW1_IC4_EXPECT				1			//1			//Use when setting I86_PIC_ICW1_MASK_IC4
#define PIC_ICW1_IC4_NO					0			//0
#define PIC_ICW1_SNGL_YES				2			//10		//Use when setting I86_PIC_ICW1_MASK_SNGL
#define PIC_ICW1_SNGL_NO				0			//00
#define PIC_ICW1_ADI_CALLINTERVAL4		4			//100		//Use when setting I86_PIC_ICW1_MASK_ADI
#define PIC_ICW1_ADI_CALLINTERVAL8		0			//000
#define PIC_ICW1_LTIM_LEVELTRIGGERED	8			//1000		//Use when setting I86_PIC_ICW1_MASK_LTIM
#define PIC_ICW1_LTIM_EDGETRIGGERED		0			//0000
#define PIC_ICW1_INIT_YES				0x10		//10000		//Use when setting I86_PIC_ICW1_MASK_INIT
#define PIC_ICW1_INIT_NO				0			//00000

//-----------------------------------------------
//	Initialization Command 4 control bits
//-----------------------------------------------

#define PIC_ICW4_UPM_86MODE			1			//1
#define PIC_ICW4_UPM_MCSMODE		0			//0
#define PIC_ICW4_AEOI_AUTOEOI		2			//10
#define PIC_ICW4_AEOI_NOAUTOEOI		0			//0
#define PIC_ICW4_MS_BUFFERMASTER	4			//100
#define PIC_ICW4_MS_BUFFERSLAVE		0			//0
#define PIC_ICW4_BUF_MODEYES		8			//1000
#define PIC_ICW4_BUF_MODENO			0			//0
#define PIC_ICW4_SFNM_NESTEDMODE	0x10		//10000
#define PIC_ICW4_SFNM_NOTNESTED		0			//a binary 2 (futurama joke hehe ;)

// enable all hardware interrupts
void enable () {
	__asm__ ("sti\n\r");
}


// disable all hardware interrupts
void disable () {
	__asm__ ("cli\n\r");
}

// send command to PICs
void pic_send_command (uint8_t cmd, uint8_t picNum) { // 2 PIC, thus picNum=0 or 1

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum==1) ? PIC2_REG_COMMAND : PIC1_REG_COMMAND;
	port_byte_out (reg, cmd);
}


// send data to PICs
void pic_send_data (uint8_t data, uint8_t picNum) {

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	port_byte_out (reg, data);
}


// read data from PICs
uint8_t pic_read_data (uint8_t picNum) {

	if (picNum > 1)
		return 0;

	uint8_t	reg = (picNum==1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	return port_byte_in (reg);
}

// Initialize pic
void pic_init () {

	uint8_t		icw	= 0;

	// disable hardware interrupts
	disable ();

	// Begin initialization of PIC, ICW1

	icw = (icw & ~PIC_ICW1_MASK_INIT) | PIC_ICW1_INIT_YES; // 00010000 10000
	// 00010000
	icw = (icw & ~PIC_ICW1_MASK_IC4) | PIC_ICW1_IC4_EXPECT; // 1 1
	// 00010001

	pic_send_command (icw, 0); // 0x11
	pic_send_command (icw, 1); // 0x11

	// Send initialization control word 2. This is the base addresses of the irq's
	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	// pic_send_data (base0, 0); // 0x20
	// pic_send_data (base1, 1); // 0x28
	pic_send_data (0x20, 0);
	pic_send_data (0x28, 1);

	// Send initialization control word 3. This is the connection between master and slave.
	// ICW3 for master PIC is the IR that connects to secondary pic in binary format
	// ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	// ???
	pic_send_data (0x04, 0);
	pic_send_data (0x02, 1);
	// pic_send_data (0x01, 0);
	// pic_send_data (0x01, 1);

	// Send Initialization control word 4. Enables i86 mode

	// ???
	icw = (icw & ~PIC_ICW4_MASK_UPM) | PIC_ICW4_UPM_86MODE; // 1 1
	// 00010001
	// icw = PIC_ICW4_UPM_86MODE; // 1

	pic_send_data (icw, 0);
	pic_send_data (icw, 1);

	enable();
}

// notifies hal interrupt is done
void interruptdone (unsigned int intno) {

	// insure its a valid hardware irq
	if (intno > 16)
		return;

	// test if we need to send end-of-interrupt to second pic
	if (intno >= 8)
		pic_send_command (PIC_OCW2_MASK_EOI, 1);

	// always send end-of-interrupt to primary pic
	pic_send_command (PIC_OCW2_MASK_EOI, 0);
}

#endif // PIC_H