#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/misc/kprintf.h>
#include <kernel/pic.h>
#include <kernel/ports.h>
#include <kernel/types.h>
#include <stdlib/malloc.h>

#define IRQ_BASE 0x20

struct interrupt_gate_desc interrupt_desc_table[256];
struct interrupt_manager* active_interrupt_manager;

void set_interrupt_desc_table_entry(struct interrupt_manager* manager,
                                    u8 interrupt_number,
                                    u16 code_segment_selector_offset,
                                    void (*handler)(), u8 privilege, u8 type) {
  interrupt_desc_table[interrupt_number].handler_address_low =
      ((u32)handler) & 0xFFFF;
  interrupt_desc_table[interrupt_number].handler_address_high =
      (((u32)handler) >> 16) & 0xFFFF;
  interrupt_desc_table[interrupt_number].gdt_code_segment_selector =
      code_segment_selector_offset;
  interrupt_desc_table[interrupt_number].access =
      0x80 | ((privilege & 3) << 5) | type;
  interrupt_desc_table[interrupt_number].reserved = 0;
}

struct interrupt_handler* new_interrupt_handler(
    u8 interrupt_number, struct interrupt_manager* interrupt_manager,
    u32 (*handle)(u32 esp)) {
  struct interrupt_handler* handler = malloc(sizeof(struct interrupt_handler));
  handler->interrupt_number = interrupt_number;
  handler->manager = interrupt_manager;
  handler->handle = handle;
  interrupt_manager->handlers[interrupt_number] = handler;
  return handler;
}

struct interrupt_manager* new_idt(struct global_descriptor_table* gdt) {
  struct interrupt_manager* manager = malloc(sizeof(struct interrupt_manager));

  u32 code_segment = code_segment_selector(gdt);
  const u8 idt_interrupt_gate = 0xE;

  kprintf("[IDT] CS=%d GATE=%d \n", code_segment, idt_interrupt_gate);

  for (u8 i = 255; i > 0; i--) {
    manager->handlers[i] = 0;
    set_interrupt_desc_table_entry(manager, i, code_segment,
                                   &ignore_interrupt_request, 0,
                                   idt_interrupt_gate);
  }

  // Requests
  set_interrupt_desc_table_entry(manager, IRQ_BASE + 0x0, code_segment,
                                 &handle_interrupt_request_0x00, 0,
                                 idt_interrupt_gate);

  set_interrupt_desc_table_entry(manager, IRQ_BASE + 0x1, code_segment,
                                 &handle_interrupt_request_0x01, 0,
                                 idt_interrupt_gate);

  // set_interrupt_desc_table_entry(manager, IRQ_BASE + 0xc, code_segment,
  //                                &handle_interrupt_request_0x0C, 0,
  //                                idt_interrupt_gate);

  // Exceptions
  // set_interrupt_desc_table_entry(manager, 0x0D, code_segment,
  //                                &handle_interrupt_exception_0x0D, 0,
  //                                idt_interrupt_gate);

  // remap irqs
  irq_remap();

  // load idt
  struct interrupt_desc_table_ptr idtr;
  idtr.size = 256 * sizeof(struct interrupt_gate_desc) - 1;
  idtr.base = (u32)interrupt_desc_table;
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

  return manager;
}

void interrupt_manager_active(struct interrupt_manager* manager) {
  if (active_interrupt_manager != 0) {
    interrupt_manager_deactive(active_interrupt_manager);
  }

  active_interrupt_manager = manager;
  asm("sti");

  kprintf("[IDT] Activated! \n");
}

void interrupt_manager_deactive(struct interrupt_manager* manager) {
  if (active_interrupt_manager == manager) {
    active_interrupt_manager = 0;
    asm("cli");
    kprintf("[IDT] Deactivated! \n");
  }
}

extern u32 interrupt_manager_handle_interrupt(u8 interrupt_number, u32 esp) {
  kprintf(
      "[IDT] interrupt_manager_handle_interrupt called with "
      "interrupt_number=%d, esp=%d\n",
      interrupt_number, esp);

  if (active_interrupt_manager != 0) {
    return interrupt_manager_do_handle_interrupt(active_interrupt_manager,
                                                 interrupt_number, esp);
  }
  return esp;
}

u32 interrupt_manager_do_handle_interrupt(struct interrupt_manager* manager,
                                          u8 interrupt_number, u32 esp) {
  if (manager->handlers[interrupt_number] != 0) {
    esp = manager->handlers[interrupt_number]->handle(esp);
  } else if (interrupt_number != 0x20) {
    kprintf("[IDT] Unhandled Interrupt: %d \n", interrupt_number);
  }

  irq_ack(interrupt_number);

  return esp;
}

// handler
void ignore_interrupt_request() {}