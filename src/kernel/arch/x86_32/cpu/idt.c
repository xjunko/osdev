#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/ports.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>

static struct interrupt_gate_desc idt_entries[256];
static struct interrupt_handler* idt_handlers[256];
static struct interrupt_manager idt;

void idt_set_entry(u8 interrupt_number, u16 code_segment_selector_offset,
                   void (*handler)(), u8 privilege, u8 type) {
  idt_entries[interrupt_number].handler_address_low = ((u32)handler) & 0xFFFF;
  idt_entries[interrupt_number].handler_address_high =
      (((u32)handler) >> 16) & 0xFFFF;
  idt_entries[interrupt_number].gdt_code_segment_selector =
      code_segment_selector_offset;
  idt_entries[interrupt_number].access = 0x80 | ((privilege & 3) << 5) | type;
  idt_entries[interrupt_number].reserved = 0;
}

void idt_set_handler(u8 interrupt_number, interrupt_callback handle) {
  struct interrupt_handler* handler = malloc(sizeof(struct interrupt_handler));
  handler->interrupt_number = interrupt_number;
  handler->handle = handle;
  idt_handlers[interrupt_number] = handler;
}

void idt_init(struct global_descriptor_table* gdt) {
  u32 code_segment = code_segment_selector(gdt);
  const u8 idt_interrupt_gate = 0xE;

  printf("[IDT] CS=%d GATE=%d \n", code_segment, idt_interrupt_gate);

  for (u8 i = 255; i > 0; i--) {
    idt_handlers[i] = 0;
    idt_set_entry(i, code_segment, &ignore_interrupt_request, 0,
                  idt_interrupt_gate);
  }

  // Requests
  idt_set_entry(IRQ_BASE + 0x0, code_segment, &handle_interrupt_request_0x00, 0,
                idt_interrupt_gate);

  idt_set_entry(IRQ_BASE + 0x1, code_segment, &handle_interrupt_request_0x01, 0,
                idt_interrupt_gate);

  idt_set_entry(IRQ_BASE + 0xc, code_segment, &handle_interrupt_request_0x0C, 0,
                idt_interrupt_gate);

  idt_set_entry(0x80, code_segment, &handle_interrupt_request_0x80, 0,
                idt_interrupt_gate);

  // remap irqs
  irq_remap();

  // load idt
  struct interrupt_desc_table_ptr idtr;
  idtr.size = 256 * sizeof(struct interrupt_gate_desc) - 1;
  idtr.base = (u32)idt_entries;
  asm volatile("lidt %0" : : "m"(idtr));

  // verify idt
  struct interrupt_desc_table_ptr after_idtr;
  asm volatile("sidt %0" : "=m"(after_idtr));

  if (idtr.size == after_idtr.size && idtr.base == after_idtr.base) {
    printf(
        "[IDT] Matching Size and Base after fetching, safe to assume "
        "everything went fine. \n");
  } else {
    // spam so that we can see it
    for (int i = 0; i < 10; i++) {
      printf(
          "[IDT] Size and Base mismatch after fetching, something went "
          "wrong. \n");
    }
  }
}

void idt_activate() {
  asm("sti");
  printf("[IDT] Activated! \n");
}

void idt_deactivate() {
  asm("cli");
  printf("[IDT] Deactivated! \n");
}

extern u32 idt_handle_interrupt(u8 interrupt_number, u32 esp) {
  printf(
      "[IDT] idt_handle_interrupt called with "
      "interrupt_number=%d, esp=%d\n",
      interrupt_number, esp);

  return _idt_handle_interrupt(interrupt_number, esp);
}

u32 _idt_handle_interrupt(u8 interrupt_number, u32 esp) {
  if (idt_handlers[interrupt_number] != 0) {
    esp = idt_handlers[interrupt_number]->handle(esp);
  } else if (interrupt_number != 0x20) {
    printf("[IDT] Unhandled Interrupt: %x \n", interrupt_number);
  }

  irq_ack(interrupt_number);

  return esp;
}

// handler
void ignore_interrupt_request() {}