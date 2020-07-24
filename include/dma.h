#ifndef DMA_H
#define DMA_H

#define DPCR *((volatile unsigned int *)(0x1F8010F0))

#define DMA3_MADR *((volatile unsigned int *)(0x1F8010B0))
#define DMA3_BCR  *((volatile unsigned int *)(0x1F8010B4))
#define DMA3_CHCR *((volatile unsigned int *)(0x1F8010B8))

#endif
