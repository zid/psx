OUTPUT_FORMAT("elf32-littlemips")

SECTIONS
{
	. = 0x8000F800;
	.text : { 
		header.o (.text);
		. = ALIGN(0x800);
		*.o (.text*);
		. = ALIGN(0x800);
	}
	.data : {
		. = ALIGN(0x800);
		*.o (.data*);
		*.o (.rodata*);
	}
	image_size = SIZEOF(.text) - 0x800;
}

