#pragma once
#include <kernel/types.h>

struct idt_entry {
  uint16_t base_low;
  uint16_t segment_selector;
  uint8_t reserved;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uintptr_t base;
} __attribute__((packed));

static struct {
  struct idt_entry entries[256];
  struct idt_ptr pointer;
} idt __attribute__((used));
#define ENTRY(X) (idt.entries[X])

typedef void (*idt_gate)(void);

void idt_install(void);
extern void idt_load(uintptr_t);
void idt_set_gate(uint8_t, idt_gate, uint16_t, uint8_t);
