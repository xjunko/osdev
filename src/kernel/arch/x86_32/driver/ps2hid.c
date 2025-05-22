#include <kernel/idt.h>
#include <kernel/misc/kprintf.h>
#include <kernel/ports.h>
#include <kernel/ps2hid.h>
#include <kernel/regs.h>
#include <kernel/types.h>

#define PS2_DATA 0x60

#define KB_IRQ 0x21
#define KB_CMD 0x64

int ps2_kb_init(struct interrupt_manager* manager) {
  new_interrupt_handler(KB_IRQ, manager, &ps2_kb_handle);

  while (inportb(KB_CMD) & 0x1) {
    inportb(PS2_DATA);
    outportb(KB_CMD, 0xAE);
    outportb(KB_CMD, 0x20);
    u8 status = (inportb(PS2_DATA) | 1) & ~0x10;
    outportb(KB_CMD, 0x60);
    outportb(PS2_DATA, status);
    outportb(PS2_DATA, 0xF4);
  }
}

u32 ps2_kb_handle(u32 esp) {
  u8 key = inportb(PS2_DATA);
  return esp;
}