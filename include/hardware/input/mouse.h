#ifndef __LOVE_HW_MOUSE_H
#define __LOVE_HW_MOUSE_H

#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/port.h>

namespace RinOS {
namespace Hardware {
namespace Driver {
class MouseDriver : public RinOS::Hardware::Communication::InterruptHandler {
  RinOS::Hardware::Communication::Port8Bit data_port;
  RinOS::Hardware::Communication::Port8Bit command_port;

  u8 buffer[3];
  u8 offset;
  u8 buttons;

  i8 x, y;

 public:
  MouseDriver(RinOS::Hardware::Communication::InterruptManager* manager);
  ~MouseDriver();

  virtual u32 handle_interrupt(u32 esp);
};
}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS
#endif