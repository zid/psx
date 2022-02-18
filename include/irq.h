#ifndef IRQ_H
#define IRQ_H
void irq_install(void);
extern void irq_callback(void);
#define IRQ_STAT *((volatile unsigned int *)(0xbf801070))
#define IRQ_MASK *((volatile unsigned int *)(0xbf801074))

#define IRQ_VBLANK 1
#define IRQ_GPU 2
#define IRQ_CDROM 4
#define IRQ_DMA 8
#endif
