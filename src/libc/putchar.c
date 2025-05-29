#include <kernel/serial.h>
#include <kernel/types.h>
#include <stdio.h>

int putchar(int c) {
  serial_putchar(c);
  return c;
}