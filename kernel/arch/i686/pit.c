#include <kernel/pit.h>
#include <kernel/ports.h>
#include <kernel/types.h>

void pit_write(u32 count) {
  asm("cli");
  outportb(PIT_CTRL, PIT_SET);
  outportb(PIT_CHAN_0, count & PIT_MASK);
  outportb(PIT_CHAN_0, (count >> 8) & PIT_MASK);
  asm("sti");
}

u32 pit_read() {
  u32 count;

  asm("cli");
  outportb(PIT_CTRL, 0b00000000);
  count = inportb(PIT_CHAN_0);
  count |= inportb(PIT_CHAN_0) << 8;
  asm("sti");

  return count;
}

void pit_sleep(u32 ms) {
  for (u32 i = 0; i < ms; i++) {
    pit_write(0x2E9A);

    u32 start = pit_read();
    while ((start - pit_read()) < 1000) {
      for (volatile u32 j = 0; j < 10; j++);
    }
  }
}