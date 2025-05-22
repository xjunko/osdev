#pragma once
#include <kernel/types.h>

struct regs {
  /* Pushed by common stub */
  u32 r15, r14, r13, r12;
  u32 r11, r10, r9, r8;
  u32 rbp, rdi, rsi, rdx, rcx, rbx, rax;

  /* Pushed by wrapper */
  u32 int_no, err_code;

  /* Pushed by interrupt */
  u32 rip, cs, rflags, rsp, ss;
};