#include "gpu.h"

void irq_install(void (*f)(void));
void irq_enable_vblank(void);

unsigned int l[] =
{
	0x08FFFFFF,	/* 8 words, last entry */
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
	gpu_send_list(l);
	//gpu_send_packet(l);
}

int main(void)
{
	irq_install(vblank_callback);

	gpu_reset();
	gpu_display_offset(0, 0);
	gpu_horiz_range(608, 608 + (320*8));
	gpu_vert_range(16, 256);
	gpu_video_mode(1);
	gpu_draw_mode(0x38F);
	gpu_clip_area(0, 0, 320, 240);
	gpu_draw_offset(0, 0);
	gpu_display_enable(0);

	irq_enable_vblank();

	while(1)
		;
}
