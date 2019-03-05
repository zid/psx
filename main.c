#include "gpu.h"

#define CD_STATUS (*(volatile unsigned char *)(0x1F801800))

void irq_install(void (*f)(void));
void irq_enable_vblank(void);

static unsigned int l2[] =
{
	0x08FFFFFF,
	0x38002040,
	0x00200020,
	0x00000020,
	0x00200130,
	0x00250010,
	0x01000020,
	0x00004000,
	0x01000130
};

static unsigned int l[] =
{
	0,	/* 8 words, last entry */
	0x38300040, 	/* 38 = 4 sided polygon*/
			/* 300040 = BGR of first vert */
	0x00000000,	/* XY of first vert */
	0x00250010,
	0x00000150,
	0x00250010,
	0x01100000,
	0x00800000,
	0x01100150
};

void vblank_callback(void)
{
	l[0] = 0x8<<24 | ((unsigned int)&l2 & 0xFFFFFF);
	gpu_send_list(l);
	//gpu_send_packet(l);

	
}

int main(void)
{
	irq_install(vblank_callback);

	const char path[] = "cdrom:\\zero2.bmp";
	int mode = 1;
	int fd;

	/* enable all irqs for fun */
	*(volatile unsigned int *)(0x1F801070) = 0;
	*(volatile unsigned int *)(0x1F801074) = 0xff;

	asm volatile (
		"addiu $t1, $0, 0x32;"    /* Function 0 */
		"addiu $t2, $0, 0xB0;" /* Bios 0xA0 */ 
		"move $a0, %1;"
		"move $a1, %2;"
		"addiu $sp, $sp, -16;"
		"nop;"
		"jalr $t2;"
		"nop;"
		"move %0, $v0;"
		"addiu $sp, $sp, 16;"
		: "=r" (fd)
		: "r" (path), "r" (mode)
		: "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$7", "$8",
		  "$9", "$10", "$11", "$12", "$13", "$14", "$15"
	);


	gpu_reset();
	gpu_display_offset(0, 0);
	gpu_horiz_range(608, 608 + (320*8));
	gpu_vert_range(16, 256);
	gpu_video_mode(0x11);
	gpu_draw_mode(0x38F);
	gpu_clip_area(0, 0, 320, 240);
	gpu_draw_offset(0, 0);
	gpu_display_enable(0);
	
	while(CD_STATUS & 0x80)
		;

	/* Init */
	CD_STATUS = 0;
	*(volatile unsigned  char *)(0x1F801801) = 0xA;

	/* Enable CD Audio */
	*(volatile unsigned short *)(0x1F801DAA) = 1 | 0x4000;

	/* CD audio volume */
	*(volatile unsigned   int *)(0x1F801DB0) = 0x40004000;
	
	/* Main volume */
	*(volatile unsigned short *)(0x1F801D80) = 0x3FFE;
	*(volatile unsigned short *)(0x1F801D82) = 0x3FFF;
	
	/* Index 2 - Left audio*/
	CD_STATUS = 2;
	*(volatile unsigned  char *)(0x1F801802) = 0x80;
	*(volatile unsigned  char *)(0x1F801803) = 0x0;
	
	/* Index 3 - Right audio */
	CD_STATUS = 3;
	*(volatile unsigned  char *)(0x1F801801) = 0x80;
	*(volatile unsigned  char *)(0x1F801802) = 0x0;
	/* Apply */
	*(volatile unsigned  char *)(0x1F801803) = 0x20;


	while(CD_STATUS & 0x80)
		;
	CD_STATUS = 0;
	*(volatile unsigned  char *)(0x1F801802) = 0x1;
	*(volatile unsigned  char *)(0x1F801801) = 0xE;

	/* Index 0 - Send Command */
	while(CD_STATUS & 0x80)
		;
	CD_STATUS = 0;
	*(volatile unsigned  char *)(0x1F801802) = 2;	/* Track 2 */
	*(volatile unsigned  char *)(0x1F801801) = 3;	/* Play */


	irq_enable_vblank();

	while(1)
		;
}
