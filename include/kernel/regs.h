#pragma once
#include <kernel/types.h>

struct regs {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;

  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;

  uint32_t error;

  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint32_t ss;
} __attribute__((packed));
