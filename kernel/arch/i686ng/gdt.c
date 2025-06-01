#include <kernel/gdt2.h>
#include <kernel/kprintf.h>
#include <kernel/regs.h>
#include <kernel/tss.h>
#include <kernel/types.h>

static struct {
  struct gdt_entry entries[6];
  struct gdt_ptr pointer;
  struct tss_entry tss;
} gdt __attribute__((used));
#define ENTRY(X) (gdt.entries[X])

void gdt_set_gate(uint8_t num, uint64_t base, uint64_t limit, uint8_t access,
                  uint8_t granularity) {
  ENTRY(num).base_low = (base & 0xFFFF);
  ENTRY(num).base_middle = (base >> 16) & 0xFF;
  ENTRY(num).base_high = (base >> 24) & 0xFF;

  ENTRY(num).limit_low = (limit & 0xFFFF);
  ENTRY(num).granularity = (limit >> 16) & 0x0F;

  ENTRY(num).granularity |= (granularity & 0xF0);

  ENTRY(num).access = access;
}

static void write_tss(int32_t num, uint8_t ss0, uint32_t esp0);

void gdt_install(void) {
  struct gdt_ptr *gdtp = &gdt.pointer;
  gdtp->limit = sizeof(gdt.entries) - 1;
  gdtp->base = (uintptr_t)&ENTRY(0);

  gdt_set_gate(0, 0, 0, 0, 0);                 // null
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // user code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // user data segment
  write_tss(5, 0x10, 0x0);

  gdt_flush((uintptr_t)gdtp);
  tss_flush();
}

static void write_tss(int32_t num, uint8_t ss0, uint32_t esp0) {
  struct tss_entry *tss = &gdt.tss;
  uintptr_t base = (uintptr_t)tss;
  uintptr_t limit = base + sizeof(*tss);

  // add the tss desc into the gdt
  gdt_set_gate(num, base, limit, 0xE9, 0x00);

  memset(tss, 0x0, sizeof(*tss));

  tss->ss0 = ss0;    // kernel stack segment
  tss->esp0 = esp0;  // kernel stack pointer
  tss->cs = 0x0b;
  tss->ss = 0x13;
  tss->ds = 0x13;
  tss->es = 0x13;
  tss->fs = 0x13;
  tss->gs = 0x13;

  tss->iomap_base = sizeof(*tss);  // i/o map base address
}

void set_kernel_stack(uintptr_t stack) { gdt.tss.esp0 = stack; }
