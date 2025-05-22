#pragma once
#include <kernel/types.h>

struct interrupt_handler {
  u8 interrupt_number;
  struct interrupt_manager* manager;
  u32 (*handle)(u32 esp);
};

struct interrupt_handler* new_interrupt_handler(u8, struct interrupt_manager*,
                                                u32 (*handle)(u32 esp));
u32 handle_interrupt(struct interrupt_handler*, u32);

struct interrupt_manager {
  struct interrupt_handler* handlers[256];
};

struct interrupt_manager* new_idt(struct global_descriptor_table*);
void interrupt_manager_active(struct interrupt_manager*);
void interrupt_manager_deactive(struct interrupt_manager*);
u32 interrupt_manager_handle_interrupt(u8, u32);

u32 interrupt_manager_do_handle_interrupt(struct interrupt_manager*, u8, u32);

void set_interrupt_desc_table_entry(struct interrupt_manager*, u8, u16,
                                    void (*handler)(), u8, u8);

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

static void ignore_interrupt_request();

static void handle_interrupt_request_0x00();
static void handle_interrupt_request_0x01();
static void handle_interrupt_request_0x0C();

static void handle_interrupt_exception_0x0D();