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

static int irq_callback(void)
{
	if(IRQ_STAT & IRQ_VBLANK)
	{
		vblank_callback();
		IRQ_STAT = ~IRQ_VBLANK;
		return 0;
	}

	return 1;
}

int main(void)
{
	irq_install(irq_callback);

	IRQ_MASK |= IRQ_VBLANK;

	gpu_reset();
	gpu_display_offset(0, 0);
	gpu_horiz_range(608, 608 + (320*8));
	gpu_vert_range(16, 256);
	gpu_video_mode(0x11);
	gpu_draw_mode(0x38F);
	gpu_clip_area(0, 0, 320, 240);
	gpu_draw_offset(0, 0);
	gpu_display_enable(0);

	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(CMD_INIT);

	/* Enable CD Audio */
	*(volatile unsigned short *)(0x1F801DAA) = 1 | 0x4000;
	/* CD audio volume */
	*(volatile unsigned   int *)(0x1F801DB0) = 0x40004000;
	/* Main volume */
	*(volatile unsigned short *)(0x1F801D80) = 0x3FFE;
	*(volatile unsigned short *)(0x1F801D82) = 0x3FFF;

	/* Set audio to max volume stereo */
	CD_WAIT();
	CD_SELECT(2);
	CD_VOL(128, 0, 128, 0);
	CD_APPLY_VOLUME();

	/* Enable CD-Audio */
	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(MODE_CCDA);
	CD_CMD(CMD_SETMODE);

	/* Play track 2 */
	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(2); /* Track number */
	CD_CMD(CMD_PLAY);

	irq_enable_vblank();
	
	while(1)
		;
}
