#pragma once
#include <kernel/types.h>

struct regs {
  u32 eax;
  u32 ebx;
  u32 ecx;
  u32 edx;

  u32 esi;
  u32 edi;
  u32 ebp;

  /*
  u32 gs;
  u32 fs;
  u32 es;
  u32 ds;
  */
  u32 error;

  u32 eip;
  u32 cs;
  u32 eflags;
  u32 esp;
  u32 ss;
} __attribute__((packed));