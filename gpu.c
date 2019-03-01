#define GP0    *((volatile unsigned int *)(0x1F801810))
#define GP1    *((volatile unsigned int *)(0x1F801814))
#define DPCR   *((volatile unsigned int *)(0x1F8010F0))
#define DICR   *((volatile unsigned int *)(0x1F8010F4))
#define D2MADR *((volatile unsigned int *)(0x1F8010A0))
#define D2BCR  *((volatile unsigned int *)(0x1F8010A4))
#define D2CHCR *((volatile unsigned int *)(0x1F8010A8))

#define DMA_START    0x01000000
#define DMA_LIST     0x00000400
#define DMA_FROM_RAM 0x00000001

static void gpu_dma_direction(unsigned int n)
{
	GP1 = 0x4<<24 | (n & 1);
}

void gpu_send_packet(unsigned int *l)
{
	int i, len = l[0]>>24;
	while((GP1 & (1<<28)) == 0)
		;
	for(i = 1; i <= len; i++)
		GP0 = l[i];
}

void gpu_send_list(void *p)
{
	/* Wait for 'DMA Ready' bit */
	while((GP1 & (1<<28)) == 0)
		;
	
	/* Enable DMA Channel 2 */
	DPCR |= 0x800;

	/* Disable DMA IRQs */
	DICR = 0;

	/* DMA cpu to gpu */
	gpu_dma_direction(2);

	D2MADR = (unsigned int)p;
	D2BCR  = 0;
	D2CHCR = DMA_START | DMA_LIST | DMA_FROM_RAM;
}

void gpu_display_enable(unsigned int n)
{
	GP1 = 0x3<<24 | (n & 1);
}

void gpu_draw_offset(int x, int y)
{
	GP0 = 0xE5<<24 | (y & 0x7FF)<<11 | (x & 0x7FF);
}

void gpu_clip_area(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2)
{
	GP0 = 0xE3<<24 | (y  & 0x1FF)<<10 | (x  & 0x3FF);
	GP0 = 0xE4<<24 | (y2 & 0x1FF)<<10 | (x2 & 0x3FF);
}

void gpu_draw_mode(unsigned int n)
{
	GP0 = 0xE1<<24 | (n & 0xFFFF);
}

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

