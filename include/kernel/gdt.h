#pragma once
#include <kernel/types.h>

struct segment_desc {
  u16 limit_low;
  u16 base_low;
  u8 base_hi;
  u8 type;
  u8 flags;
  u8 base_vhigh;
} __attribute__((packed));

struct segment_desc new_segment_desc(u32, u32, u8);
u32 segment_desc_base(struct segment_desc*);
u32 segment_desc_limit(struct segment_desc*);

struct global_descriptor_table {
  struct segment_desc null_segment_selector;
  struct segment_desc unused_segment_selector;
  struct segment_desc code_segment_selector;
  struct segment_desc data_segment_selector;
};

void gdt_init();
u16 code_segment_selector();
u16 data_segment_selector();