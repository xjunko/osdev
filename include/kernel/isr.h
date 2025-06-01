#pragma once
#include <kernel/types.h>

#define ISR(ISR) extern void _isr##ISR(void)
#define ISR_SET(ISR)     \
  isrs[ISR].index = ISR; \
  isrs[ISR].stub = _isr##ISR

typedef void (*isr_callback)(void);

// bit cursed but, whatever.
ISR(0);
ISR(1);
ISR(2);
ISR(3);
ISR(4);
ISR(5);
ISR(6);
ISR(7);
ISR(8);
ISR(9);
ISR(10);

void isr_install(void);
void isr_install_handler(size_t, isr_callback);
void isr_uninstall_handler(size_t);