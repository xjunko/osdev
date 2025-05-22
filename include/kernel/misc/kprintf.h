#pragma once
#include <kernel/types.h>

void _init_dev_serial();
bool _dev_serial_ready();
void _dev_serial_putchar(char c);
volatile int kprintf(const char*, ...);