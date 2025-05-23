#pragma once
#include <kernel/gdt.h>
#include <kernel/types.h>

struct interrupt_handler {
  u8 interrupt_number;
  struct interrupt_manager* manager;
  u32 (*handle)(u32 esp);
};

struct interrupt_manager {
  struct interrupt_handler* handlers[256];
};

void idt_init(struct global_descriptor_table*);
void idt_activate();
void idt_deactivate();
void idt_set_entry(u8, u16, void (*handler)(), u8, u8);
void idt_set_handler(u8, u32 (*handle)(u32 esp));
u32 idt_handle_interrupt(u8, u32);
u32 _idt_handle_interrupt(u8, u32);

struct interrupt_gate_desc {
  u16 handler_address_low;
  u16 gdt_code_segment_selector;
  u8 reserved;
  u8 access;
  u16 handler_address_high;
} __attribute__((packed));

struct interrupt_desc_table_ptr {
  u16 size;
  u32 base;
} __attribute__((packed));

void ignore_interrupt_request();

void handle_interrupt_request_0x00();
void handle_interrupt_request_0x01();
void handle_interrupt_request_0x0C();
void handle_interrupt_exception_0x0D();