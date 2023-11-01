
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../asm/common.c"

uint32_t pit_read_count() {
  uint32_t count = 0;

  asm("cli"); // disable interrupts.
  asm_outb(0x43, 0b0000000);

  count = asm_inb(0x40);
  count |= asm_inb(0x40) << 8;

  return count;
}