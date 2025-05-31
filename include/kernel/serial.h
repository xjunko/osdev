#include <kernel/types.h>

void serial_init();
bool serial_ready();
void serial_putchar(uint8_t c);