#include <kernel/idt.h>
#include <kernel/regs.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>

// linux style, this should be enough for now
#define SYSCALL_N 420
static syscall_callback* syscall_handlers[SYSCALL_N];

void syscall_install();

void syscall_init(u8 interrupt_number) {
  idt_set_handler(IRQ_BASE + interrupt_number, syscall_handle);
  syscall_install();
}

void syscall_register(u32 syscall_number, syscall_callback* handler) {
  if (syscall_number < SYSCALL_N) {
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

// implementation
#define SYSCALL_EXIT 0x1
static u32 syscall_exit(struct regs* r) {
  printf("[syscall] exit!");
  return (u32)r;
}

#define SYSCALL_FORK 0x2
static u32 syscall_fork(struct regs* r) {
  printf("[syscall] fork!");
  return (u32)r;
}

#define SYSCALL_READ 0x3
static u32 syscall_read(struct regs* r) {
  printf("[syscall] read!");
  return (u32)r;
}

#define SYSCALL_WRITE 0x4
static u32 syscall_write(struct regs* r) {
  printf("[syscall] write!");
  return (u32)r;
}

// install
void syscall_install() {
  syscall_register(SYSCALL_EXIT, syscall_exit);
  syscall_register(SYSCALL_FORK, syscall_fork);
  syscall_register(SYSCALL_READ, syscall_read);
  syscall_register(SYSCALL_WRITE, syscall_write);
}