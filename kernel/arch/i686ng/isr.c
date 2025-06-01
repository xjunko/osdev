#include <kernel/isr.h>
#include <kernel/regs2.h>
#include <kernel/types.h>

#define IST_COUNT 32

static struct {
  size_t index;
  void (*stub)(void);
} isrs[32 + 1] __attribute__((used));

static isr_callback isr_callbacks[256] = {0};

void isr_install_handler(size_t isr, isr_callback callback) {
  isr_callbacks[isr] = callback;
}

void isr_uninstall_handler(size_t isr) { isr_callbacks[isr] = 0; }

void isr_install(void) {
  ISR_SET(0);
  ISR_SET(1);
  ISR_SET(2);
  ISR_SET(3);
  ISR_SET(4);
  ISR_SET(5);
  ISR_SET(6);
  ISR_SET(7);
  ISR_SET(8);
  ISR_SET(9);
  ISR_SET(10);

  for (int i = 0; i < 11; i++) {
    idt_set_gate(isrs[i].index, isrs[i].stub, 0x08, 0x8E);
  }
}

void isr_handler(struct regs *r) {
  isr_callback handler = isr_callbacks[r->int_no];
  if (handler) {
    handler();
  } else {
    kprintf("[isr] unhandled interrupt: %d\n", r->int_no);
  }
}