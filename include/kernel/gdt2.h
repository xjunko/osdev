#pragma once
#include <kernel/tss.h>
#include <kernel/types.h>

struct gdt_entry {
  // limits
  uint16_t limit_low;
  // segment addr
  uint16_t base_low;
  uint8_t base_middle;
  // access mode
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
  uint16_t limit;
  uintptr_t base;
} __attribute__((packed));

void gdt_install(void);
void gdt_set_gate(uint8_t, uint64_t, uint64_t, uint8_t, uint8_t);
extern void gdt_flush(uintptr_t);
void set_kernel_stack(uintptr_t);
