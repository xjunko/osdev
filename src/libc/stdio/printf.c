// a really scuffed kprintf, logs to serial port com 1

#include <kernel/ports.h>
#include <kernel/serial.h>
#include <kernel/types.h>
#include <stdio.h>

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

// putchars
void kputchar(int c, void* extras) {
  static int at_line_start = 1;
  if (at_line_start && c != '\n') {
    serial_putchar('\033');
    serial_putchar('[');
    serial_putchar('3');
    serial_putchar('4');
    serial_putchar('m');
    at_line_start = 0;
  }
  serial_putchar(c);
  if (c == '\n') {
    serial_putchar('\033');
    serial_putchar('[');
    serial_putchar('0');
    serial_putchar('m');
    at_line_start = 1;
  }
}

void print_string(const char* str) {
  while (*str) {
    putchar(*str++);
  }
}

void print_decimal(int value) {
  if (value < 0) {
    putchar('-');
    value = -value;
  }

  char buffer[10];
  int i = 0;

  if (value == 0) {
    putchar('0');
    return;
  }

  while (value > 0) {
    buffer[i++] = '0' + (value % 10);
    value /= 10;
  }

  while (i--) {
    putchar(buffer[i]);
  }
}

void print_hex(unsigned int value) {
  char hex_digits[] = "0123456789ABCDEF";
  print_string("0x");

  int started = 0;
  for (int i = 28; i >= 0; i -= 4) {
    char digit = (value >> i) & 0xF;
    if (digit || started || i == 0) {
      putchar(hex_digits[(u8)digit]);
      started = 1;
    }
  }
}

int printf(string __restrict__ fmt, ...) {
  va_list args;
  va_start(args, fmt);

  int written = 0;

  while (*fmt) {
    if (*fmt == '%' && *(fmt + 1)) {
      fmt++;
      switch (*fmt) {
        case 'd':
          print_decimal(va_arg(args, int));
          break;
        case 's':
          print_string(va_arg(args, const char*));
          break;
        case 'x':
          print_hex(va_arg(args, unsigned int));
          break;
        case 'X':
          print_hex(va_arg(args, unsigned int));
          break;
        default:
          putchar('%');
          putchar(*fmt);
          break;
      }
    } else {
      putchar(*fmt);
    }
    fmt++;
    written++;
  }

  va_end(args);
  return written;
}