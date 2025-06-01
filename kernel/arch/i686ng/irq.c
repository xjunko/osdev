#include <kernel/irq.h>
#include <kernel/ports.h>
#include <kernel/types.h>

// pic
#define PIC1 0x20
#define PIC1_CMD PIC1
#define PIC1_OFFSET 0x20
#define PIC1_DATA (PIC1 + 1)

#define PIC2 0xA0
#define PIC2_CMD PIC2
#define PIC2_OFFSET 0x28
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01

// interrupts
static volatile int sync_depth = 0;

void int_disable(void) {
  uint32_t flags;
  asm volatile(
      "pushf\n\t"
      "pop %%eax\n\t"
      "movl %%eax,%0\n\t"
      : "=r"(flags)::"%eax");
  asm volatile("cli");

  if (flags & (1 << 9)) {
    sync_depth = 1;
  } else {
    sync_depth++;
  }
}

void int_resume(void) {
  if (sync_depth == 0 || sync_depth == 1) {
    asm volatile("sti");
  } else {
    sync_depth--;
  }
}

void int_enable(void) {
  sync_depth = 0;
  asm volatile("sti");
}

// requests
#define IRQ_CHAIN_SIZE 16
#define IRQ_CHAIN_DEPTH 4

static void (*irqs[IRQ_CHAIN_SIZE])(void);
static irq_handler_chain irq_routines[IRQ_CHAIN_SIZE * IRQ_CHAIN_DEPTH] = {0};
static char* _irq_description[IRQ_CHAIN_SIZE * IRQ_CHAIN_DEPTH] = {0};

char* irq_get_handler(int irq, int chain) {
  if (irq >= IRQ_CHAIN_SIZE) return 0;
  if (chain >= IRQ_CHAIN_DEPTH) return 0;
  return _irq_description[IRQ_CHAIN_SIZE * chain + irq];
}

void irq_install_handler(int irq, irq_handler_chain handler,
                         char* description) {
  asm volatile("cli");
  for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++) {
    if (irq_routines[i * IRQ_CHAIN_SIZE + irq]) {
      continue;
    }
    irq_routines[i * IRQ_CHAIN_SIZE + irq] = handler;
    _irq_description[i * IRQ_CHAIN_SIZE + irq] = description;
    break;
  }
  asm volatile("sti");
}

void irq_uninstall_handler(int irq) {
  asm volatile("cli");
  for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++) {
    irq_routines[i * IRQ_CHAIN_SIZE + irq] = 0;
  }
  asm volatile("sti");
}

static void irq_remap() {
  // init
  outportbslow(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
  outportbslow(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

  // remap
  outportbslow(PIC1_DATA, PIC1_OFFSET);
  outportbslow(PIC2_DATA, PIC2_OFFSET);

  // ident
  outportbslow(PIC1_DATA, 0x04);
  outportbslow(PIC2_DATA, 0x02);

  // 8086 mode
  outportbslow(PIC1_DATA, 0x01);
  outportbslow(PIC2_DATA, 0x01);
}

static void irq_setup_gates(void) {
  for (size_t i = 0; i < IRQ_CHAIN_SIZE; i++) {
    idt_set_gate(32 + i, irqs[i], 0x08, 0x8E);
  }
}

void irq_install(void) {
  IRQ_SET(0);
  IRQ_SET(1);
  IRQ_SET(2);
  IRQ_SET(3);
  IRQ_SET(4);
  IRQ_SET(5);
  IRQ_SET(6);
  IRQ_SET(7);
  IRQ_SET(8);
  IRQ_SET(9);
  IRQ_SET(10);
  IRQ_SET(11);
  IRQ_SET(12);
  IRQ_SET(13);
  IRQ_SET(14);
  IRQ_SET(15);
  irq_remap();
  irq_setup_gates();
}

void irq_ack(size_t irq) {
  if (irq >= 8) {
    outportb(PIC2_CMD, PIC_EOI);
  }
  outportb(PIC1_CMD, PIC_EOI);
}

void irq_handler(struct regs* r) {
  int_disable();
  if (r->int_no <= 47 && r->int_no >= 32) {
    for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++) {
      irq_handler_chain handler =
          irq_routines[i * IRQ_CHAIN_SIZE + (r->int_no - 32)];
      if (!handler) break;
      if (handler(r)) {
        goto handled;
      }
    }
    irq_ack(r->int_no - 32);
  }
handled:
  int_resume();
}