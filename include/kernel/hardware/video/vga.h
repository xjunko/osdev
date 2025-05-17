#ifndef __LOVE_HW_VGA_H
#define __LOVE_HW_VGA_H

#include <commons/types.h>
#include <kernel/hardware/communication/port.h>

namespace RinOS {
namespace Hardware {
namespace Driver {
class VGA {
 protected:
  RinOS::Hardware::Communication::Port8Bit misc_port;
  RinOS::Hardware::Communication::Port8Bit dac_index_port;
  RinOS::Hardware::Communication::Port8Bit dac_data_port;
  RinOS::Hardware::Communication::Port8Bit crtc_index_port;
  RinOS::Hardware::Communication::Port8Bit crtc_data_port;
  RinOS::Hardware::Communication::Port8Bit sequencer_index_port;
  RinOS::Hardware::Communication::Port8Bit sequencer_data_port;
  RinOS::Hardware::Communication::Port8Bit graphics_controller_index_port;
  RinOS::Hardware::Communication::Port8Bit graphics_controller_data_port;
  RinOS::Hardware::Communication::Port8Bit attribute_controller_index_port;
  RinOS::Hardware::Communication::Port8Bit attribute_controller_read_port;
  RinOS::Hardware::Communication::Port8Bit attribute_controller_write_port;
  RinOS::Hardware::Communication::Port8Bit attribute_controller_reset_port;

  void WriteRegisters(u8* registers);
  u8* GetFrameBufferPart();

  virtual u8 GetColorIndex(u8 r, u8 g, u8 b);

 public:
  VGA();
  ~VGA();

  u8 vga_pallete[256][3];

  virtual bool IsSupported(u32 width, u32 height, u32 color_depth);
  virtual bool SetMode(u32 width, u32 height, u32 color_depth);
  virtual void PutPixel(u32 x, u32 y, u32 color_index);
  virtual void PutPixel(u32 x, u32 y, u8 r, u8 g, u8 b);
};
}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS

#endif