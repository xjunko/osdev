#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kmalloc.h>
#include <kernel/kprintf.h>
#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/ports.h>
#include <kernel/regs.h>
#include <kernel/tasks.h>
#include <kernel/types.h>
#include <stdlib.h>

#define IDT_EXCEPTION(irq)                                                 \
  idt_set_entry((irq), code_segment, &handle_interrupt_exception_##irq, 0, \
                idt_interrupt_gate)

#define IDT_REQUEST(irq)                        \
  idt_set_entry(IRQ_BASE + (irq), code_segment, \
                &handle_interrupt_request_##irq, 0, idt_interrupt_gate)

#define IDT_REQUEST_PRIV(irq, priv)             \
  idt_set_entry(IRQ_BASE + (irq), code_segment, \
                &handle_interrupt_request_##irq, priv, idt_interrupt_gate)

static struct interrupt_gate_desc idt_entries[256];
static struct interrupt_handler idt_handlers_stack[256];
static struct interrupt_handler* idt_handlers[256];
static struct interrupt_manager idt;

void idt_set_entry(uint8_t interrupt_number,
                   uint16_t code_segment_selector_offset, void (*handler)(),
                   uint8_t privilege, uint8_t type) {
  idt_entries[interrupt_number].handler_address_low =
      ((uint32_t)handler) & 0xFFFF;
  idt_entries[interrupt_number].handler_address_high =
      (((uint32_t)handler) >> 16) & 0xFFFF;
  idt_entries[interrupt_number].gdt_code_segment_selector =
      code_segment_selector_offset;
  idt_entries[interrupt_number].access = 0x80 | ((privilege & 3) << 5) | type;
  idt_entries[interrupt_number].reserved = 0;
}

void idt_set_handler(uint8_t interrupt_number, interrupt_callback handle) {
  idt_handlers_stack[interrupt_number].interrupt_number = interrupt_number;
  idt_handlers_stack[interrupt_number].handle = handle;
  idt_handlers[interrupt_number] = &idt_handlers_stack[interrupt_number];
}

uint32_t idt_general_protection_fault(uint32_t esp) {
  struct regs* r = (struct regs*)esp;
  uint32_t error_code = r->error;
  uint32_t eip = r->eip;        // EIP pushed by CPU
  uint32_t cs = r->cs;          // CS pushed by CPU
  uint32_t eflags = r->eflags;  // EFLAGS pushed by CPU

  kprintf("[idt] General Protection Fault!\n");
  kprintf("Error code: %X\n", error_code);
  kprintf("EIP: %X\n", eip);
  kprintf("CS: %X\n", cs);
  kprintf("EFLAGS: %X\n", eflags);

  asm("hlt");
  return esp;
}

void idt_init() {
  uint32_t code_segment = code_segment_selector();
  const uint8_t idt_interrupt_gate = 0xE;

  kprintf("[IDT] CS=%x GATE=%x \n", code_segment, idt_interrupt_gate);

  for (uint8_t i = 255; i > 0; i--) {
    idt_handlers[i] = 0;
    idt_set_entry(i, code_segment, &ignore_interrupt_request, 0,
                  idt_interrupt_gate);
  }

  // Exceptions
  IDT_EXCEPTION(0x00);
  IDT_EXCEPTION(0x01);
  IDT_EXCEPTION(0x02);
  IDT_EXCEPTION(0x03);
  IDT_EXCEPTION(0x04);
  IDT_EXCEPTION(0x05);
  IDT_EXCEPTION(0x06);
  IDT_EXCEPTION(0x07);
  IDT_EXCEPTION(0x08);
  IDT_EXCEPTION(0x09);
  IDT_EXCEPTION(0x0A);
  IDT_EXCEPTION(0x0B);
  IDT_EXCEPTION(0x0C);
  IDT_EXCEPTION(0x0D);
  IDT_EXCEPTION(0x0E);
  IDT_EXCEPTION(0x0F);
  IDT_EXCEPTION(0x10);
  IDT_EXCEPTION(0x11);
  IDT_EXCEPTION(0x12);
  IDT_EXCEPTION(0x13);

  // Requests
  IDT_REQUEST(0x00);
  IDT_REQUEST(0x01);
  IDT_REQUEST(0x02);
  IDT_REQUEST(0x03);
  IDT_REQUEST(0x04);
  IDT_REQUEST(0x05);
  IDT_REQUEST(0x06);
  IDT_REQUEST(0x07);
  IDT_REQUEST(0x08);
  IDT_REQUEST(0x09);
  IDT_REQUEST(0x0A);
  IDT_REQUEST(0x0C);

  IDT_REQUEST_PRIV(0x60, 0x3);

  // in the rare event that this might happen
  idt_set_handler(0xD, idt_general_protection_fault);

  // remap irqs
  irq_remap();
  pit_write(PIT_HZ / PIT_SCALE);

  // load idt
  struct interrupt_desc_table_ptr idtr;
  idtr.size = 256 * sizeof(struct interrupt_gate_desc) - 1;
  idtr.base = (uint32_t)idt_entries;
  asm volatile("lidt %0" : : "m"(idtr));

  // verify idt
  struct interrupt_desc_table_ptr after_idtr;
  asm volatile("sidt %0" : "=m"(after_idtr));

  if (idtr.size == after_idtr.size && idtr.base == after_idtr.base) {
    kprintf(
        "[IDT] Matching Size and Base after fetching, safe to assume "
        "everything went fine. \n");
  } else {
    // spam so that we can see it
    for (int i = 0; i < 10; i++) {
      kprintf(
          "[IDT] Size and Base mismatch after fetching, something went "
          "wrong. \n");
    }
  }
}

void idt_activate() {
  asm("sti");
  kprintf("[IDT] Activated! \n");
}

void idt_deactivate() {
  asm("cli");
  kprintf("[IDT] Deactivated! \n");
}

extern uint32_t idt_handle_interrupt(uint8_t interrupt_number, uint32_t esp) {
#ifdef KERNEL_DEBUG
  kprintf(
      "[IDT] idt_handle_interrupt called with "
      "interrupt_number=%x, esp=%d\n",
      interrupt_number, esp);
#endif
  return _idt_handle_interrupt(interrupt_number, esp);
}

uint32_t _idt_handle_interrupt(uint8_t interrupt_number, uint32_t esp) {
  if (idt_handlers[interrupt_number] != 0) {
    esp = idt_handlers[interrupt_number]->handle(esp);
  } else if (interrupt_number != 0x20 && interrupt_number != 0x2e) {
    struct regs* r = (struct regs*)esp;
    kprintf("[IDT] Unhandled Interrupt: %x", interrupt_number);
    kprintf(" | a=%d b=%x c=%x d=%x \n", r->eax, r->ebx, r->ecx, r->edx);
  }

  if (interrupt_number == 0x20) {
    asm volatile("cli");
    esp = cpu_schedule(esp);
    asm volatile("sti");
  }

  irq_ack(interrupt_number);

  return esp;
}

// handler
void ignore_interrupt_request() {}