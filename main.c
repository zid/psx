#include "print.h"
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
	printf("IRQ: %x\n", IRQ_STAT);
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

static void voice_setvol(int n, int vol)
{
	*(volatile unsigned short *)(0x1F801C00+(n*0x10)) = vol;
	*(volatile unsigned short *)(0x1F801C02+(n*0x10)) = vol;
}

int main(void)
{
	int i;
	printf("Muting voices\n");
	for(i = 0; i < 24; i++)
		voice_setvol(i, 0);
	printf("Installing IRQ\n");
	irq_install();

	printf("Unmasking CD and VBLANK IRQs\n");
	IRQ_MASK = IRQ_CDROM | IRQ_VBLANK;
	printf("CD-ROM Init\n");
	cdrom_init();

	*((volatile unsigned int *)(0x1F80101C)) = 0x80777;
	*((volatile unsigned int *)(0x1F801114)) = 3;
	printf("GPU Reset\n");
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
	for(i = 0; i < 16; i++)
	{
		printf("Reading CD-ROM sector: 0:02:%d\n", int2bcd(28+i));
		cdrom_read_sect(0, 2, int2bcd(28+i));
		printf("Copying rect to VRAM (640, %d)\n", 256+(i*8));
		gpu_copy_rect_from_cd(640, 256+(i*8), 128, 8);
	}

	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(MODE_CCDA | 2);
	CD_CMD(CMD_SETMODE);

	printf("Playing audio track\n");
	/* Play track 2 */
	CD_WAIT();
	CD_SELECT(0);
	CD_EAT_FIFO();
	CD_PARAM(2); /* Track number */
	CD_CMD(CMD_PLAY);

	printf("Setting vblank callback to zero2\n");
	vblank_callback = late_callback;

	while(1)
		;
}
