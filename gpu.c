#define GP0 *((volatile unsigned int *)(0x1F801810))
#define GP1 *((volatile unsigned int *)(0x1F801814))

void gpu_video_mode(unsigned int n)
{
	GP1 = 0x8<<24 | (n & 0xF);
}

void gpu_vert_range(unsigned int top, unsigned int bottom)
{
	GP1 = 0x7<<24 | (bottom & 0x1FF)<<10 | (top & 0x1FF);
}

void gpu_horiz_range(unsigned int start, unsigned int end)
{
	GP1 = 0x6<<24 | (end & 0xFFF)<<12 | (start & 0xFFF);
}

void gpu_display_offset(unsigned int x, unsigned int y)
{
	GP1 = (0x5<<24) | (y & 0x1FF)<<10 | (x & 0x3FF);
}

void gpu_reset(void)
{
	GP1 = 0;
}

