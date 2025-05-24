#pragma once
#include <kernel/multiboot.h>
#include <kernel/types.h>

#define VESA_MAGIC "VESA"
#define VESA_MAGIC_SIZE 4
#define VESA_MODE_INVALID 0xffff
#define VESA_MODE_TERMINATOR 0xffff
#define VESA_MODE_LINEAR_FRAMEBUFFER 0x4000
#define VESA_MODE_ATTR_GRAPHICS 0x10
#define VESA_MODE_ATTR_LINEAR_FRAMEBUFFER 0x80
#define VESA_MODE_MEMORY_MODEL_DIRECTCOLOR 0x6

typedef u32 vesa_color;

struct vesa_info_t {
  char magic[VESA_MAGIC_SIZE];
  u16 version;
  u16 oem_string_ptr[2];
  u8 capabilities[4];
  u16 video_mode_ptr[2];
  u16 total_memory;
  u8 reserved[492];
} __attribute__((packed));

struct vesa_mode_info_t {
  u16 attr;
  u8 win_a, win_b;
  u16 win_granularity;
  u16 win_size;
  u16 seg_a, seg_b;
  u32 real_mode_func_ptr;
  u16 pitch;

  u16 x_res, y_res;
  u8 w_char, y_char, planes, bpp, banks;
  u8 memory_model, bank_size, image_pages;
  u8 reserved0;

  u8 red_mask, red_position;
  u8 green_mask, green_position;
  u8 blue_mask, blue_position;
  u8 rsv_mask, rsv_position;
  u8 directcolor_attr;

  u32 base;
  u32 reserved1;
  u16 reserved2;
  u8 reserved[206];
} __attribute__((packed));

void vesa_init(multiboot_info_t*);
u32 vesa_device_color(vesa_color);
void vesa_clear(vesa_color);
