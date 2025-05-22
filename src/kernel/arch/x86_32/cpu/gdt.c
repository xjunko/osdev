#include <kernel/gdt.h>
#include <kernel/misc/kprintf.h>
#include <kernel/types.h>
#include <stdlib/malloc.h>

struct segment_desc new_segment_desc(u32 base, u32 limit, u8 flags) {
  struct segment_desc desc;

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

struct global_descriptor_table* new_gdt() {
  struct global_descriptor_table* gdt =
      malloc(sizeof(struct global_descriptor_table));

  kprintf("[GDT] Segment Init: ");
  gdt->null_segment_selector = new_segment_desc(0, 0, 0);
  gdt->unused_segment_selector = new_segment_desc(0, 0, 0);
  gdt->code_segment_selector = new_segment_desc(0, 64 * 1024 * 1024, 0x9A);
  gdt->data_segment_selector = new_segment_desc(0, 64 * 1024 * 1024, 0x92);
  kprintf("Passed! \n");

  u32 i[2];
  i[1] = (u32)gdt;
  i[0] = sizeof(struct global_descriptor_table) << 16;
  asm volatile("lgdt (%0)" : : "p"(((u8*)i) + 2));

  kprintf("[GDT] Initialized! \n");

  return gdt;
}

u16 code_segment_selector(struct global_descriptor_table* gdt) {
  //   return (u8*)gdt->code_segment_selector - (u8*)gdt;
  return (u8*)&gdt->code_segment_selector - (u8*)gdt;
}

u16 data_segment_selector(struct global_descriptor_table* gdt) {
  //  return (u8*)gdt->data_segment_selector - (u8*)gdt;
  return (u8*)&gdt->data_segment_selector - (u8*)gdt;
}
