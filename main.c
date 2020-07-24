#include "gpu.h"
#include "cdrom.h"
#include "irq.h"

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
	0x38300040,	/* 38 = 4 sided polygon*/
			/* 300040 = BGR of first vert */
	0x00000000,	/* XY of first vert */
	0x00250010,
	0x00000150,
	0x00250010,
	0x01100000,
	0x00800000,
	0x01100150
};

static void vblank_callback()
{
	l[0] = 0x8<<24 | ((unsigned int)&l2 & 0xFFFFFF);
	gpu_send_list(l);
}

void irq_callback(void)
{
	if(IRQ_STAT & IRQ_VBLANK)
	{
		vblank_callback();
		IRQ_STAT = ~IRQ_VBLANK;
	}

	if(IRQ_STAT & IRQ_CDROM)
	{
		cdrom_callback();
		IRQ_STAT = ~IRQ_CDROM;
	}
}

int main(void)
{
	irq_install();

	IRQ_MASK |= IRQ_CDROM;

	*((volatile unsigned int *)(0x1F80101C)) = 0x80777;
	*((volatile unsigned int *)(0x1F801114)) = 3;

	gpu_reset();
	gpu_display_offset(0, 0);
	gpu_horiz_range(608, 608 + (320*8));
	gpu_vert_range(16, 256);
	gpu_video_mode(0x11);
	gpu_draw_mode(0x38F);
	gpu_clip_area(0, 0, 320, 240);
	gpu_draw_offset(0, 0);
	gpu_display_enable(0);
	
	#define int2bcd(x) ((x)/10)*16 + ((x)%10)
	int i;
	for(i = 0; i < 16; i++)
	{
		cdrom_read_sect(0, 2, int2bcd(28+i));
		gpu_copy_rect_from_cd(800, 200+(i*8), 128, 8);
	}
#if 0
	/* Play track 2 */
	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(2); /* Track number */
	CD_CMD(CMD_PLAY);
#endif

	IRQ_MASK |= IRQ_VBLANK;

	while(1)
		;
}
