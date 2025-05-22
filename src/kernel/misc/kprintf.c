// a really scuffed kprintf, logs to serial port com 1

#include <kernel/ports.h>
#include <kernel/types.h>
// #include <stdarg.h>
#define NANOPRINTF_IMPLEMENTATION
#include <library/nanoprintf.h>

// Serial
const u16 COM1 = 0x3F8;
bool _dev_serial_init = false;

#define _queue_size 1024
static u8 _queue[_queue_size];  // ought to be enough!
static int _queue_i = 0;

void _init_dev_serial() {
  outportb(COM1 + 1, 0x00);
  outportb(COM1 + 3, 0x80);
  outportb(COM1 + 0, 0x03);
  outportb(COM1 + 1, 0x00);
  outportb(COM1 + 3, 0x03);
  outportb(COM1 + 2, 0xC7);
  outportb(COM1 + 4, 0x0B);

  _dev_serial_init = true;
}

bool _dev_serial_ready() { return (inportb(COM1 + 5) & 0x20) != 0; }

void _dev_serial_putchar(u8 c) {
  if (!_dev_serial_ready() || !_dev_serial_init) {
    if (_queue_i >= _queue_size) {
      return;  // sue me
    }
    _queue[_queue_i] = c;
    _queue_i++;
    return;
  }

  // clear queue first
  if (_queue_i != 0) {
    for (int i = 0; i < _queue_i; i++) {
      outportb(COM1, _queue[i]);
      _queue[i] = 0;
    }
    _queue_i = 0;
  }

  outportb(COM1, c);
}

// VGA
static u16* video_mem = (u16*)0xb8000;
const u16 video_w = 80;
const u16 video_h = 25;

static u8 term_x = 0;
static u8 term_y = 0;
static u8 vga_color = 0x0C;

void _vga_putchar(char c) {
  if (c == '\n') {
    term_x = 0;
    term_y++;
    return;
  }

  video_mem[term_y * video_w + term_x] = ((u16)vga_color << 8) | c;
  term_x++;

  if (term_x >= 80) {
    term_y++;
    term_x = 0;
  }

  if (term_y > 25) {
    term_x = 0;
    term_y = 0;
  }
}

// putchars
void vgaputchar(int c, void* extras) { _vga_putchar(c); }
void serialputchar(int c, void* extras) {
  static int at_line_start = 1;
  if (at_line_start && c != '\n') {
    _dev_serial_putchar('\033');
    _dev_serial_putchar('[');
    _dev_serial_putchar('3');
    _dev_serial_putchar('4');
    _dev_serial_putchar('m');
    at_line_start = 0;
  }
  _dev_serial_putchar(c);
  if (c == '\n') {
    _dev_serial_putchar('\033');
    _dev_serial_putchar('[');
    _dev_serial_putchar('0');
    _dev_serial_putchar('m');
    at_line_start = 1;
  }
}

int kprintf(string fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  int ret = npf_pprintf(serialputchar, NULL, fmt, args);
  __builtin_va_end(args);
  return ret;
}