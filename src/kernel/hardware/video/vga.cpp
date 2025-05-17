#include <commons/types.h>
#include <kernel/hardware/communication/port.h>
#include <kernel/hardware/communication/serial/serial.h>
#include <kernel/hardware/video/vga.h>
#include <kernel/terminal/term.h>

using namespace RinOS::Hardware::Driver;
using namespace RinOS::Hardware::Communication::Serial;

VGA::VGA()
    : misc_port(0x3c2),
      dac_index_port(0x3C7),
      dac_data_port(0x3C9),
      crtc_index_port(0x3d4),
      crtc_data_port(0x3d5),
      sequencer_index_port(0x3c4),
      sequencer_data_port(0x3c5),
      graphics_controller_index_port(0x3ce),
      graphics_controller_data_port(0x3cf),
      attribute_controller_index_port(0x3c0),
      attribute_controller_read_port(0x3c1),
      attribute_controller_write_port(0x3c0),
      attribute_controller_reset_port(0x3da) {
  COM1.Print("[VGA] Initialized.");

  COM1.Print("[VGA] Reading VGA Pallete.");
  this->dac_index_port.Write(0);

  for (int i = 0; i < 256; i++) {
    vga_pallete[i][0] = this->dac_data_port.Read();
    vga_pallete[i][1] = this->dac_data_port.Read();
    vga_pallete[i][2] = this->dac_data_port.Read();
  }

  COM1.Print("[VGA] VGA Pallete, filled the entire array!");
}

VGA::~VGA() {}

void VGA::WriteRegisters(u8* registers) {
  //  misc
  misc_port.Write(*(registers++));

  // sequencer
  for (u8 i = 0; i < 5; i++) {
    sequencer_index_port.Write(i);
    sequencer_data_port.Write(*(registers++));
  }

  // cathode ray tube controller
  crtc_index_port.Write(0x03);
  crtc_data_port.Write(crtc_data_port.Read() | 0x80);
  crtc_index_port.Write(0x11);
  crtc_data_port.Write(crtc_data_port.Read() & ~0x80);

  registers[0x03] = registers[0x03] | 0x80;
  registers[0x11] = registers[0x11] & ~0x80;

  for (u8 i = 0; i < 25; i++) {
    crtc_index_port.Write(i);
    crtc_data_port.Write(*(registers++));
  }

  // graphics controller
  for (u8 i = 0; i < 9; i++) {
    graphics_controller_index_port.Write(i);
    graphics_controller_data_port.Write(*(registers++));
  }

  // attribute controller
  for (u8 i = 0; i < 21; i++) {
    attribute_controller_reset_port.Read();
    attribute_controller_index_port.Write(i);
    attribute_controller_write_port.Write(*(registers++));
  }

  attribute_controller_read_port.Read();
  attribute_controller_index_port.Write(0x20);
}

// Dummy implementation
bool VGA::IsSupported(u32 width, u32 height, u32 color_depth) { return true; }

bool VGA::SetMode(u32 width, u32 height, u32 colordepth) {
  COM1.Print("[VGA] ModeSet!");
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
  WriteRegisters(g_320x200x256);  // 8bit color depth!
  return true;
}

u8* VGA::GetFrameBufferPart() {
  graphics_controller_index_port.Write(0x06);
  u8 segmentNumber = graphics_controller_data_port.Read() & (3 << 2);
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

void VGA::PutPixel(u32 x, u32 y, u32 colorIndex) {
  u8* pixelAddress = GetFrameBufferPart() + 320 * y + x;
  *pixelAddress = colorIndex;
}

u8 VGA::GetColorIndex(u8 r, u8 g, u8 b) {
  int i;
  int best_index = 0;
  int best_distance = 256 * 256 * 3;

  r >>= 2;
  g >>= 2;
  b >>= 2;

  for (i = 0; i < 256; i++) {
    int dr = (int)r - (int)this->vga_pallete[i][0];
    int dg = (int)g - (int)this->vga_pallete[i][1];
    int db = (int)b - (int)this->vga_pallete[i][2];

    int distance = dr * dr + dg * dg + db * db;

    if (distance == 0) return (u8)i;

    if (distance < best_distance) {
      best_distance = distance;
      best_index = i;
    }
  }

  return (u8)best_index;
}

void VGA::PutPixel(u32 x, u32 y, u8 r, u8 g, u8 b) {
  PutPixel(x, y, GetColorIndex(r, g, b));
}