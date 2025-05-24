#include <kernel/idt.h>
#include <kernel/regs.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>

// linux has 256, this should be enough for now
#define SYSCALL_N 256
static syscall_callback* syscall_handlers[SYSCALL_N];

void syscall_init(u8 interrupt_number) {
  idt_set_handler(IRQ_BASE + interrupt_number, syscall_handle);
}

void syscall_register(u32 syscall_number, syscall_callback* handler) {
  if (syscall_number < 256) {
    syscall_handlers[syscall_number] = handler;
  } else {
    printf("[SYSCALL] Invalid syscall number: %d\n", syscall_number);
  }
}

u32 syscall_handle(u32 esp) {
  struct regs* r = (struct regs*)esp;

  u32 syscall_number = r->eax;

  if (syscall_handlers[syscall_number] != 0) {
    return (*syscall_handlers[syscall_number])(r);
  } else {
    printf("[SYSCALL] Unhandled syscall: %d\n", syscall_number);
  }

  return esp;
}
