/*
* link.ld
*/

OUTPUT_FORMAT("elf32-i386")

ENTRY(main)

SECTIONS
{
	. = 0x80000;

	.text ALIGN (0x00) :
	{
		*(.text);
	}
	
	.rodata ALIGN (4) : {
		*(.rodata*);
	}

	.data ALIGN (4) : {
		*(.data*);
	}

	.bss ALIGN (4) : {
		*(COMMON);
		*(.bss*);
	}

	/DISCARD/ : {
		*(.comment);
		*(.eh_frame);
	}
}