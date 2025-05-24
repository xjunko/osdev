#include <kernel/idt.h>
#include <kernel/regs.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>

void syscall_init(u8 interrupt_number) {
  idt_set_handler(IRQ_BASE + interrupt_number, syscall_handle);
}

// todo: handle syscalls
u32 syscall_handle(u32 esp) {
  struct regs* r = (struct regs*)esp;

  switch (r->eax) {
    default:
      break;
  }

  return esp;
}