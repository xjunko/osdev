#include <kernel/pit.h>
#include <kernel/ports.h>
#include <kernel/types.h>

void pit_write(uint32_t count) {
  asm("cli");
  outportb(PIT_CTRL, PIT_SET);
  outportb(PIT_CHAN_0, count & PIT_MASK);
  outportb(PIT_CHAN_0, (count >> 8) & PIT_MASK);
  asm("sti");
}

uint32_t pit_read() {
  uint32_t count;

  asm("cli");
  outportb(PIT_CTRL, 0x0);
  count = inportb(PIT_CHAN_0);
  count |= inportb(PIT_CHAN_0) << 8;
  asm("sti");

  return count;
}

void pit_sleep(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i++) {
    pit_write(0x2E9A);

    uint32_t start = pit_read();
    while ((start - pit_read()) < 1000) {
      for (volatile uint32_t j = 0; j < 10; j++);
    }
  }
}