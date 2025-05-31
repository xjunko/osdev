#include <kernel/gdt.h>
#include <kernel/kprintf.h>
#include <kernel/types.h>

static struct global_descriptor_table gdt;

struct segment_desc new_segment_desc(u32 base, u32 limit, u8 flags) {
  struct segment_desc desc = {};

  u8* target = (u8*)&desc;

  if (limit <= 65536) {
    target[6] = 0x40;
  } else {
    if ((limit & 0xFFF) != 0xFFF) {
      limit = (limit >> 12) - 1;
    } else {
      limit = limit >> 12;
    }

    target[6] = 0xC0;
  }

  target[0] = limit & 0xFF;
  target[1] = (limit >> 8) & 0xFF;
  target[6] |= (limit >> 16) & 0xF;

  target[2] = base & 0xFF;
  target[3] = (base >> 8) & 0xFF;
  target[4] = (base >> 16) & 0xFF;
  target[7] = (base >> 24) & 0xFF;

  target[5] = flags;

  return desc;
}

u32 segment_desc_base(struct segment_desc* desc) {
  u8* target = (u8*)desc;
  u32 result = target[7];
  result = (result << 8) + target[4];
  result = (result << 8) + target[3];
  result = (result << 8) + target[2];
  return result;
}

u32 segment_desc_limit(struct segment_desc* desc) {
  u8* target = (u8*)desc;
  u32 result = target[6] & 0xF;
  result = (result << 8) + target[1];
  result = (result << 8) + target[0];

  if ((target[6] & 0xC0) == 0xC0) {
    result = (result << 12) | 0xFFF;
  }

  return result;
}

void gdt_init() {
  kprintf("[GDT] Segment Init: ");
  gdt.null_segment_selector = new_segment_desc(0, 0, 0);
  gdt.unused_segment_selector = new_segment_desc(0, 0, 0);
  gdt.code_segment_selector = new_segment_desc(0, 64 * 1024 * 1024, 0x9A);
  gdt.data_segment_selector = new_segment_desc(0, 64 * 1024 * 1024, 0x92);
  kprintf("Passed! \n");

  struct {
    u16 limit;
    u32 base;
  } __attribute__((packed)) gdtr;
  gdtr.limit = sizeof(struct global_descriptor_table) - 1;
  gdtr.base = (u32)&gdt;
  asm volatile("lgdt %0" : : "m"(gdtr));

  kprintf("[GDT] Initialized! \n");
}

u16 code_segment_selector() { return 2 * sizeof(struct segment_desc); }

u16 data_segment_selector() { return 3 * sizeof(struct segment_desc); }
