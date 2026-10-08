#ifndef PLIC_H
#define OLIC_H
#define UART_IRQ 10

void plic_init(void);
unsigned int plic_claim(void);
void plic_complete(unsigned int irq);
#endif