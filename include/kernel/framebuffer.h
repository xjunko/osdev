#pragma once
#include <kernel/multiboot.h>
#include <kernel/types.h>

struct framebuffer_info {
  void *addr;                    // framebuffer address
  size_t width;                  // width in pixels
  size_t height;                 // height in pixels
  size_t bpp;                    // bits per pixel
  size_t pitch;                  // bytes per line
  uint8_t red_field_position;    // red field position
  uint8_t red_mask_size;         // red mask size
  uint8_t green_field_position;  // green field position
  uint8_t green_mask_size;       // green mask size
  uint8_t blue_field_position;   // blue field position
  uint8_t blue_mask_size;        // blue mask size
};

void framebuffer_init(struct multiboot_tag_framebuffer *);
struct framebuffer_info framebuffer_get_info();

void framebuffer_set_pixel(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void framebuffer_clear(uint32_t, uint32_t, uint32_t);
void framebuffer_flush();

void framebuffer_fill_rect(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                           uint32_t, uint32_t);
