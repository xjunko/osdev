#include <kernel/idt2.h>
#include <kernel/types.h>

void idt_set_gate(uint8_t num, idt_gate base, uint16_t segment_selector,
                  uint8_t flags) {
  ENTRY(num).base_low = (uintptr_t)base & 0xFFFF;
  ENTRY(num).base_high = ((uintptr_t)base >> 16) & 0xFFFF;
  ENTRY(num).segment_selector = segment_selector;
  ENTRY(num).reserved = 0;
  ENTRY(num).flags = flags | 0x60;
}

void idt_install(void) {
  struct idt_ptr *idtp = &idt.pointer;
  idtp->limit = sizeof(idt.entries) - 1;
  idtp->base = (uintptr_t)&ENTRY(0);
  memset(&ENTRY(0), 0, sizeof(idt.entries));

  idt_load((uintptr_t)idtp);
}