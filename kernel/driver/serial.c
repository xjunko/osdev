#include <kernel/ports.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/types.h>

#define COM1 0x3F8
static bool _dev_init = false;

#define _queue_size 256
static uint8_t _queue[_queue_size];  // ought to be enough!
static int _queue_i = 0;

void serial_init() {
  outportb(COM1 + 1, 0x00);
  outportb(COM1 + 3, 0x80);
  outportb(COM1 + 0, 0x03);
  outportb(COM1 + 1, 0x00);
  outportb(COM1 + 3, 0x03);
  outportb(COM1 + 2, 0xC7);
  outportb(COM1 + 4, 0x0B);

  _dev_init = true;
}

bool serial_ready() { return (inportb(COM1 + 5) & 0x20) != 0; }

void serial_putchar(uint8_t c) {
  tty_write((const char *)&c);

  if (!serial_ready() || !_dev_init) {
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