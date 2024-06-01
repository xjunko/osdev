#ifndef __LOVE_HW_KEYBOARD_H
#define __LOVE_HW_KEYBOARD_H

#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/port.h>

namespace RinOS {
namespace Hardware {
namespace Driver {
class KeyboardDriver : public RinOS::Hardware::Communication::InterruptHandler {
  RinOS::Hardware::Communication::Port8Bit data_port;
  RinOS::Hardware::Communication::Port8Bit command_port;

 public:
  KeyboardDriver(RinOS::Hardware::Communication::InterruptManager* manager);
  ~KeyboardDriver();

  virtual u32 handle_interrupt(u32 esp);
};

}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS

#endif