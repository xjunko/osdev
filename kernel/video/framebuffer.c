// framebuffer.c - grub's framebuffer driver - acts like a linear framebuffer
#include <kernel/framebuffer.h>
#include <kernel/kprintf.h>
#include <kernel/types.h>
#include <multiboot2.h>
#include <stdlib.h>
#include <string.h>

static void *fb_addr;
static void *fb_backbuffer;
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_bpp;
static uint32_t fb_pitch;

static struct framebuffer_info fb_info;

#define FB_DRAW_TO fb_backbuffer

// we're booting thru grub, so we don't have any say about the resolution, we
// just rawdog it.
void framebuffer_init(struct multiboot_tag_framebuffer *fb) {
  if (fb->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
    fb_addr = (void *)fb->common.framebuffer_addr;
    fb_width = fb->common.framebuffer_width;
    fb_height = fb->common.framebuffer_height;
    fb_bpp = fb->common.framebuffer_bpp;
    fb_pitch = fb->common.framebuffer_pitch;

    // info
    fb_info.addr = fb_addr;
    fb_info.width = fb_width;
    fb_info.height = fb_height;
    fb_info.bpp = fb_bpp;
    fb_info.pitch = fb_pitch;
    fb_info.red_field_position = fb->framebuffer_red_field_position;
    fb_info.red_mask_size = fb->framebuffer_red_mask_size;
    fb_info.green_field_position = fb->framebuffer_green_field_position;
    fb_info.green_mask_size = fb->framebuffer_green_mask_size;
    fb_info.blue_field_position = fb->framebuffer_blue_field_position;
    fb_info.blue_mask_size = fb->framebuffer_blue_mask_size;
  }

  kprintf("[fb] w=%d h=%d bpp=%d \n", fb_width, fb_height, fb_bpp);
  fb_backbuffer = malloc(fb_pitch * fb_height);
  framebuffer_clear(255, 255, 255);
  framebuffer_flush();
  return;
}

struct framebuffer_info framebuffer_get_info() { return fb_info; }

void framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                           uint32_t b) {
  if (x >= fb_width || y >= fb_height) return;

  uint32_t *pixel = (uint32_t *)(FB_DRAW_TO + y * fb_pitch + x * (fb_bpp / 8));
  *pixel = (255 << 24) | (r << 16) | (g << 8) | b;  // ARGB
}

void framebuffer_clear(uint32_t r, uint32_t g, uint32_t b) {
  for (uint32_t y = 0; y < fb_height; y++) {
    for (uint32_t x = 0; x < fb_width; x++) {
      framebuffer_set_pixel(x, y, r, g, b);
    }
  }
}

void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t width,
                           uint32_t height, uint32_t r, uint32_t g,
                           uint32_t b) {
  for (uint32_t j = 0; j < height; j++) {
    for (uint32_t i = 0; i < width; i++) {
      framebuffer_set_pixel(x + i, y + j, r, g, b);
    }
  }
}

void framebuffer_flush() {
  if (fb_addr == 0 || FB_DRAW_TO == 0) return;
  if (fb_addr == FB_DRAW_TO) {
    return;
  }
  memcpy(fb_addr, FB_DRAW_TO, fb_pitch * fb_height);
}