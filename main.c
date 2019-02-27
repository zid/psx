#include "gpu.h"

void irq_install(void (*f)(void));

void vblank_callback(void)
{
	while(1)
		;
}

int main(void)
{
	irq_install(vblank_callback);

	gpu_reset();
	gpu_display_offset(0, 0);
	gpu_horiz_range(608, 608 + (320*8));
	gpu_vert_range(16, 256);
	gpu_video_mode(1);

	while(1)
		;
}
