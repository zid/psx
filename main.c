#include "gpu.h"
#include "cdrom.h"
#include "irq.h"

static unsigned int l2[] =
{
	0x09FFFFFF,
	0x2D000000,
	0x00100010, /* v1 */
	0x00000000,
	0x00100090, /* v2 */
	0x011A00FF,
	0x00900010, /* v4 */
	0x0000FF00,
	0x00900090, /* v3 */
	0x0000FFFF
};

static unsigned int l[] =
{
	0x0,
	0x38300040, /* 38 = 4 sided polygon */
	            /* 300040 = BGR of first vert */
	0x00000000, /* XY of first vert */
	0x00250010,
	0x00000150,
	0x00250010,
	0x01100000,
	0x00800000,
	0x01100150
};

static volatile int vblank;

static void early_callback()
{
	l[0] = 0x08FFFFFF;
	gpu_send_list(l);
}

static void late_callback()
{
	l[0] = (0x08<<24) | (((unsigned int)&l2) & 0xFFFFFF);
	gpu_send_list(l);
}

static void (*vblank_callback)(void) = early_callback;

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

	IRQ_MASK = IRQ_CDROM | IRQ_VBLANK;
	cdrom_init();

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
		gpu_copy_rect_from_cd(640, 256+(i*8), 128, 8);
	}

	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(MODE_CCDA | 2);
	CD_CMD(CMD_SETMODE);

	/* Play track 2 */
	CD_WAIT();
	CD_SELECT(0);
	CD_EAT_FIFO();
	CD_PARAM(2); /* Track number */
	CD_CMD(CMD_PLAY);

	vblank_callback = late_callback;

	while(1)
		;
}
