#pragma once
#include <kernel/types.h>

#define IRQ_BASE 0x20

#define IDT_EXCEPTION_DEF(irq) void handle_interrupt_exception_##irq()
#define IDT_REQUEST_DEF(irq) void handle_interrupt_request_##irq()

typedef u32 (*interrupt_callback)(u32 esp);

struct interrupt_handler {
  u8 interrupt_number;
  interrupt_callback handle;
};

struct interrupt_manager {
  struct interrupt_handler* handlers[256];
};

void idt_init();
void idt_activate();
void idt_deactivate();
void idt_set_entry(u8, u16, void (*handler)(), u8, u8);
void idt_set_handler(u8, interrupt_callback);
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

// handled in irq.s
IDT_REQUEST_DEF(0x00);
IDT_REQUEST_DEF(0x01);
IDT_REQUEST_DEF(0x02);
IDT_REQUEST_DEF(0x03);
IDT_REQUEST_DEF(0x04);
IDT_REQUEST_DEF(0x05);
IDT_REQUEST_DEF(0x06);
IDT_REQUEST_DEF(0x07);
IDT_REQUEST_DEF(0x08);
IDT_REQUEST_DEF(0x09);
IDT_REQUEST_DEF(0x0A);
IDT_REQUEST_DEF(0x0C);
IDT_REQUEST_DEF(0x60);

IDT_EXCEPTION_DEF(0x00);
IDT_EXCEPTION_DEF(0x01);
IDT_EXCEPTION_DEF(0x02);
IDT_EXCEPTION_DEF(0x03);
IDT_EXCEPTION_DEF(0x04);
IDT_EXCEPTION_DEF(0x05);
IDT_EXCEPTION_DEF(0x06);
IDT_EXCEPTION_DEF(0x07);
IDT_EXCEPTION_DEF(0x08);
IDT_EXCEPTION_DEF(0x09);
IDT_EXCEPTION_DEF(0x0A);
IDT_EXCEPTION_DEF(0x0B);
IDT_EXCEPTION_DEF(0x0C);
IDT_EXCEPTION_DEF(0x0D);
IDT_EXCEPTION_DEF(0x0E);
IDT_EXCEPTION_DEF(0x0F);
IDT_EXCEPTION_DEF(0x10);
IDT_EXCEPTION_DEF(0x11);
IDT_EXCEPTION_DEF(0x12);
IDT_EXCEPTION_DEF(0x13);