// vga.c - super basic 320x200 8bit driver, based on wyoos tutorial
#include <kernel/ports.h>
#include <kernel/types.h>
#include <kernel/vga.h>
#include <stdio.h>

#define VGA_MISC_PORT 0x3c2
#define VGA_DAC_INDEX_PORT 0x3C7
#define VGA_DAC_DATA_PORT 0x3C9
#define VGA_CRTC_INDEX_PORT 0x3d4
#define VGA_CRTC_DATA_PORT 0x3d5
#define VGA_SEQUENCER_INDEX_PORT 0x3c4
#define VGA_SEQUENCER_DATA_PORT 0x3c5
#define VGA_GRAPHICS_CONTROLLER_INDEX_PORT 0x3ce
#define VGA_GRAPHICS_CONTROLLER_DATA_PORT 0x3cf
#define VGA_ATTRIBUTE_CONTROLLER_INDEX_PORT 0x3c0
#define VGA_ATTRIBUTE_CONTROLLER_READ_PORT 0x3c1
#define VGA_ATTRIBUTE_CONTROLLER_WRITE_PORT 0x3c0
#define VGA_ATTRIBUTE_CONTROLLER_RESET_PORT 0x3da

#define VGA_COLOR_PALLETE_MASK_WRITE 0x3C6
#define VGA_COLOR_DATA_WRITE 0x3C9
#define VGA_COLOR_REGISTER_WRITE 0x3C8

static uint8_t vga_pallete[256][3];
static uint8_t vga_buffer[320 * 200];

// 320x200 8bit color mode
static uint8_t vga_default_mode[] = {
    0x63,
    /* seq */
    0x03, 0x01, 0x0f, 0x00, 0x0e,
    /* crtc */
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f, 0x00, 0x41, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9c, 0x0e, 0x8f, 0x28, 0x40, 0x96, 0xb9, 0xa3,
    0xff,
    /* gc */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff,
    /* ac */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00, 0x00};

void vga_init() {
  outportb(VGA_DAC_DATA_PORT, 0);
  for (int i = 0; i < 256; i++) {
    vga_pallete[i][0] = inportb(VGA_DAC_DATA_PORT);
    vga_pallete[i][1] = inportb(VGA_DAC_DATA_PORT);
    vga_pallete[i][2] = inportb(VGA_DAC_DATA_PORT);
  }

  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 200; y++) {
      vga_buffer[x + y * 320] = 0;
    }
  }

  printf("[VGA] Pallete initialized.\n");
}

void vga_write_registers(uint8_t* registers) {
  outportb(VGA_MISC_PORT, *(registers++));

  // sequencer
  for (uint8_t i = 0; i < 5; i++) {
    outportb(VGA_SEQUENCER_INDEX_PORT, i);
    outportb(VGA_SEQUENCER_DATA_PORT, *(registers++));
  }

  // crtc
  outportb(VGA_CRTC_INDEX_PORT, 0x03);
  outportb(VGA_CRTC_DATA_PORT, inportb(VGA_CRTC_DATA_PORT) | 0x80);
  outportb(VGA_CRTC_INDEX_PORT, 0x11);
  outportb(VGA_CRTC_DATA_PORT, inportb(VGA_CRTC_DATA_PORT) & ~0x80);

  registers[0x03] = registers[0x03] | 0x80;
  registers[0x11] = registers[0x11] & ~0x80;

  for (uint8_t i = 0; i < 25; i++) {
    outportb(VGA_CRTC_INDEX_PORT, i);
    outportb(VGA_CRTC_DATA_PORT, *(registers++));
  }

  // graphics controller
  for (uint8_t i = 0; i < 9; i++) {
    outportb(VGA_GRAPHICS_CONTROLLER_INDEX_PORT, i);
    outportb(VGA_GRAPHICS_CONTROLLER_DATA_PORT, *(registers++));
  }

  // attribute controller
  for (uint8_t i = 0; i < 21; i++) {
    inportb(VGA_ATTRIBUTE_CONTROLLER_RESET_PORT);
    outportb(VGA_ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
    outportb(VGA_ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
  }

  // reset
  inportb(VGA_ATTRIBUTE_CONTROLLER_RESET_PORT);
  outportb(VGA_ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

void vga_set_mode(uint32_t width, uint32_t height, uint32_t color_depth) {
  // force overwrite these for now.
  width = 320;
  height = 200;
  color_depth = 8;

  printf("[VGA] SetMode called with %d x %d x %d\n", width, height,
         color_depth);

  vga_write_registers(vga_default_mode);  // 8bit color depth!
}

static inline uint8_t square_diff(uint8_t a, uint8_t b) {
  int diff = (int)a - (int)b;
  return diff * diff;
}

uint8_t vga_get_color_index(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t best_index = 0;
  int best_distance = 255 * 255 * 3;

  for (uint8_t i = 0; i < 255; ++i) {
    int dr = (int)vga_pallete[i][0] - r;
    int dg = (int)vga_pallete[i][1] - g;
    int db = (int)vga_pallete[i][2] - b;
    int distance = dr * dr + dg * dg + db * db;

    if (distance < best_distance) {
      best_distance = distance;
      best_index = i;

      if (distance == 0) break;
    }
  }

  return best_index;
}

uint8_t* vga_get_frame_buffer_part() {
  outportb(VGA_GRAPHICS_CONTROLLER_INDEX_PORT, 0x06);
  uint8_t segmentNumber = inportb(VGA_GRAPHICS_CONTROLLER_DATA_PORT) & (3 << 2);
  switch (segmentNumber) {
    default:
    case 0 << 2:
      return (uint8_t*)0x00000;
    case 1 << 2:
      return (uint8_t*)0xA0000;
    case 2 << 2:
      return (uint8_t*)0xB0000;
    case 3 << 2:
      return (uint8_t*)0xB8000;
  }
}

void vga_put_color_index(uint32_t x, uint32_t y, uint32_t color_index) {
  uint8_t* pixel_address =
      vga_get_frame_buffer_part() + ((y << 8) + (y << 6) + x);
  *pixel_address = color_index;
}

// G B R
void vga_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t color_index = vga_get_color_index(g, b, r);
  vga_buffer[(y << 8) + (y << 6) + x] = color_index;
}

void vga_draw() {
  uint8_t* pixel_address = vga_get_frame_buffer_part();
  for (int i = 0; i < 320 * 200; i++) {
    *(pixel_address + i) = vga_buffer[i];
  }
}