#pragma once
#include <kernel/multiboot.h>
#include <kernel/types.h>

struct framebuffer_info {
  u8 *addr;                 // framebuffer address
  u32 width;                // width in pixels
  u32 height;               // height in pixels
  u32 bpp;                  // bits per pixel
  u32 pitch;                // bytes per line
  u8 red_field_position;    // red field position
  u8 red_mask_size;         // red mask size
  u8 green_field_position;  // green field position
  u8 green_mask_size;       // green mask size
  u8 blue_field_position;   // blue field position
  u8 blue_mask_size;        // blue mask size
};

void framebuffer_init(struct multiboot_tag_framebuffer *);
struct framebuffer_info framebuffer_get_info();

void framebuffer_set_pixel(u32, u32, u32, u32, u32);
void framebuffer_clear(u32, u32, u32);
void framebuffer_flush();

void framebuffer_fill_rect(u32, u32, u32, u32, u32, u32, u32);
