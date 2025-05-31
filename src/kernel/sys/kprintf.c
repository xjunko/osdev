// a really scuffed kprintf, logs to serial port com 1

#include <kernel/ports.h>
#include <kernel/serial.h>
#include <kernel/types.h>
#include <stdio.h>

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

int kputchar(int c) {
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
  return c;
}

void print_string(const char* str) {
  while (*str) {
    kputchar(*str++);
  }
}

void print_decimal(int value) {
  if (value < 0) {
    kputchar('-');
    value = -value;
  }

  char buffer[10];
  int i = 0;

  if (value == 0) {
    kputchar('0');
    return;
  }

  while (value > 0) {
    buffer[i++] = '0' + (value % 10);
    value /= 10;
  }

  while (i--) {
    kputchar(buffer[i]);
  }
}

void print_float(float value, int precision) {
  if (value < 0) {
    kputchar('-');
    value = -value;
  }

  int int_part = (int)value;
  float fraction = value - int_part;

  print_decimal(int_part);

  kputchar('.');

  for (int i = 0; i < precision; ++i) {
    fraction *= 10;
    int digit = (int)fraction;
    kputchar('0' + digit);
    fraction -= digit;
  }
}

void print_hex(unsigned int value) {
  char hex_digits[] = "0123456789ABCDEF";
  print_string("0x");

  int started = 0;
  for (int i = 28; i >= 0; i -= 4) {
    char digit = (value >> i) & 0xF;
    if (digit || started || i == 0) {
      kputchar(hex_digits[(u8)digit]);
      started = 1;
    }
  }
}

int kprintf(string __restrict__ fmt, ...) {
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
        case 'f':
          print_float(va_arg(args, double), 6);  // autoconvert float to
          break;
        default:
          kputchar('%');
          kputchar(*fmt);
          break;
      }
    } else {
      kputchar(*fmt);
    }
    fmt++;
    written++;
  }

  va_end(args);
  return written;
}