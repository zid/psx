OUTPUT_FORMAT("elf32-littlemips")

SECTIONS
{
	. = 0x8000F800;
	.text : { 
		header.o (.text);
		. = ALIGN(0x800);
		* (.text*);
		. = ALIGN(0x800);
		* (.data);
		* (.rodata*);
		. = ALIGN(0x800);
		/* TODO: Clear this from startup code instead of including it in the binary */
		* (.bss);
		. = ALIGN(0x800);
	}

	image_size = SIZEOF(.text) - 0x800;

	/DISCARD/ : { * (.MIPS.abiflags); }
}

