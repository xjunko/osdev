#include <kernel/serial.h>
#include <kernel/types.h>
#include <libc/stdio.h>

int putchar(int c) {
  serial_putchar(c);
  return c;
}