#include <kernel/misc/kprintf.h>
#include <kernel/ports.h>
#include <kernel/types.h>
#include <kernel/vga.h>

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

static u8 vga_pallete[256][3];

void vga_init() {
  outportb(VGA_DAC_DATA_PORT, 0);
  for (int i = 0; i < 256; i++) {
    vga_pallete[i][0] = inportb(VGA_DAC_DATA_PORT);
    vga_pallete[i][1] = inportb(VGA_DAC_DATA_PORT);
    vga_pallete[i][2] = inportb(VGA_DAC_DATA_PORT);
  }
  kprintf("[VGA] Pallete initialized.\n");
}
void vga_write_registers(u8* registers) {
  outportb(VGA_MISC_PORT, *(registers++));

  // sequencer
  for (u8 i = 0; i < 5; i++) {
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

  for (u8 i = 0; i < 25; i++) {
    outportb(VGA_CRTC_INDEX_PORT, i);
    outportb(VGA_CRTC_DATA_PORT, *(registers++));
  }

  // graphics controller
  for (u8 i = 0; i < 9; i++) {
    outportb(VGA_GRAPHICS_CONTROLLER_INDEX_PORT, i);
    outportb(VGA_GRAPHICS_CONTROLLER_DATA_PORT, *(registers++));
  }

  // attribute controller
  for (u8 i = 0; i < 21; i++) {
    inportb(VGA_ATTRIBUTE_CONTROLLER_RESET_PORT);
    outportb(VGA_ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
    outportb(VGA_ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
  }

  // reset
  inportb(VGA_ATTRIBUTE_CONTROLLER_RESET_PORT);
  outportb(VGA_ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

void vga_set_mode(u32 width, u32 height, u32 color_depth) {
  // force overwrite these for now.
  width = 320;
  height = 200;
  color_depth = 8;

  kprintf("[VGA] SetMode called with %d x %d x %d\n", width, height,
          color_depth);

  static u8 g_320x200x256[] = {
      /* MISC */
      0x63,
      /* SEQ */
      0x03, 0x01, 0x0F, 0x00, 0x0E,
      /* CRTC */
      0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
      0xFF,
      /* GC */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
      /* AC */
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
      0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00};
  vga_write_registers(g_320x200x256);  // 8bit color depth!
}

u8 vga_get_color_index(u8 r, u8 g, u8 b) {
  int i;
  int best_index = 0;
  int best_distance = 256 * 256 * 3;

  r >>= 2;
  g >>= 2;
  b >>= 2;

  for (i = 0; i < 256; i++) {
    int dr = (int)r - (int)vga_pallete[i][0];
    int dg = (int)g - (int)vga_pallete[i][1];
    int db = (int)b - (int)vga_pallete[i][2];

    int distance = dr * dr + dg * dg + db * db;

    if (distance == 0) return (u8)i;

    if (distance < best_distance) {
      best_distance = distance;
      best_index = i;
    }
  }

  return (u8)best_index;
}

u8* vga_get_frame_buffer_part() {
  outportb(VGA_GRAPHICS_CONTROLLER_INDEX_PORT, 0x06);
  u8 segmentNumber = inportb(VGA_GRAPHICS_CONTROLLER_DATA_PORT) & (3 << 2);
  switch (segmentNumber) {
    default:
    case 0 << 2:
      return (u8*)0x00000;
    case 1 << 2:
      return (u8*)0xA0000;
    case 2 << 2:
      return (u8*)0xB0000;
    case 3 << 2:
      return (u8*)0xB8000;
  }
}

void vga_put_color_index(u32 x, u32 y, u32 color_index) {
  u8* pixel_address = vga_get_frame_buffer_part() + 320 * y + x;
  *pixel_address = color_index;
}

void vga_put_pixel(u32 x, u32 y, u8 r, u8 g, u8 b) {
  u8 color_index = vga_get_color_index(r, g, b);
  vga_put_color_index(x, y, color_index);
}