#include <kernel/idt.h>
#include <kernel/regs.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>

void syscall_init(u8 interrupt_number) {
  idt_set_handler(IRQ_BASE + interrupt_number, syscall_handle);
}

// handlers
void sys_exit(struct regs* r) {
  // ebx - int
}

void sys_fork(struct regs* r) {
  // ebx - pt_regs
}

void sys_read(struct regs* r) {
  // ebx - file_descriptor
  // ecx - buffer
  // edx - size
}

void sys_write(struct regs* r) {
  // ebx - file_descriptor
  // ecx - buffer
  // edx - size
}

void sys_close(struct regs* r) {
  // ebx - file_descriptor
}

// todo: handle syscalls, linux style
u32 syscall_handle(u32 esp) {
  struct regs* r = (struct regs*)esp;

  switch (r->eax) {
    case 1:
      sys_exit(r);
      break;
    case 2:
      sys_fork(r);
      break;
    case 3:
      sys_read(r);
      break;
    case 4:
      sys_write(r);
      break;
    case 5:
      sys_close(r);
      break;
    default:
      break;
  }

  return esp;
}
