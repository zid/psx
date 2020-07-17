#ifndef IRQ_H
#define IRQ_H
void irq_install(int (*f)(void));
void irq_enable_vblank(void);

#define IRQ_STAT *((volatile unsigned int *)(0x1f801070))
#define IRQ_MASK *((volatile unsigned int *)(0x1f801074))

#define IRQ_VBLANK 1
#define IRQ_CDROM 4
#endif
