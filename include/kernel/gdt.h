#pragma once
#include <kernel/types.h>

struct segment_desc {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_hi;
  uint8_t type;
  uint8_t flags;
  uint8_t base_vhigh;
} __attribute__((packed));

struct segment_desc new_segment_desc(uint32_t, uint32_t, uint8_t);
uint32_t segment_desc_base(struct segment_desc*);
uint32_t segment_desc_limit(struct segment_desc*);

struct global_descriptor_table {
  struct segment_desc null_segment_selector;
  struct segment_desc unused_segment_selector;
  struct segment_desc code_segment_selector;
  struct segment_desc data_segment_selector;
};

void gdt_init();
uint16_t code_segment_selector();
uint16_t data_segment_selector();