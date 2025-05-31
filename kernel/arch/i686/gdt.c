#include <kernel/gdt.h>
#include <kernel/kprintf.h>
#include <kernel/types.h>

static struct global_descriptor_table gdt;

struct segment_desc new_segment_desc(uint32_t base, uint32_t limit,
                                     uint8_t flags) {
  struct segment_desc desc;

  uint8_t* target = (uint8_t*)&desc;

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

uint32_t segment_desc_base(struct segment_desc* desc) {
  uint8_t* target = (uint8_t*)desc;
  uint32_t result = target[7];
  result = (result << 8) + target[4];
  result = (result << 8) + target[3];
  result = (result << 8) + target[2];
  return result;
}

uint32_t segment_desc_limit(struct segment_desc* desc) {
  uint8_t* target = (uint8_t*)desc;
  uint32_t result = target[6] & 0xF;
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
    uint16_t limit;
    uint32_t base;
  } __attribute__((packed)) gdtr;
  gdtr.limit = sizeof(struct global_descriptor_table) - 1;
  gdtr.base = (uint32_t)&gdt;
  asm volatile("lgdt %0" : : "m"(gdtr));

  kprintf("[GDT] Initialized! \n");
}

uint16_t code_segment_selector() { return 2 * sizeof(struct segment_desc); }

uint16_t data_segment_selector() { return 3 * sizeof(struct segment_desc); }
