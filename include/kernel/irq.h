#pragma once
#include <kernel/regs2.h>
#include <kernel/types.h>

#define IRQ(N) extern void _irq##N(void)
#define IRQ_SET(N) irqs[N] = _irq##N

typedef int (*irq_handler_chain)(struct regs*);

IRQ(0);
IRQ(1);
IRQ(2);
IRQ(3);
IRQ(4);
IRQ(5);
IRQ(6);
IRQ(7);
IRQ(8);
IRQ(9);
IRQ(10);
IRQ(11);
IRQ(12);
IRQ(13);
IRQ(14);
IRQ(15);

void irq_install(void);
void irq_install_handler(int, irq_handler_chain, char*);
void irq_uninstall_handler(int);
char* irq_get_handler(int, int);
void irq_ack(size_t);