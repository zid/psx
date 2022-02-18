#ifndef DMA_H
#define DMA_H

#define DPCR *((volatile unsigned int *)(0xBF8010F0))
#define DICR *((volatile unsigned int *)(0xBF8010F4))

#define DMA3_MADR *((volatile unsigned int *)(0xBF8010B0))
#define DMA3_BCR  *((volatile unsigned int *)(0xBF8010B4))
#define DMA3_CHCR *((volatile unsigned int *)(0xBF8010B8))

#endif
