#ifndef __LOVE_HW_KEYBOARD_H
#define __LOVE_HW_KEYBOARD_H

#include <global/types.h>
#include <kernel/hardware/communication/interrupts.h>
#include <kernel/hardware/communication/port.h>
#include <kernel/hardware/driver/driver.h>

namespace RinOS {
namespace Hardware {
namespace Driver {

class KeyboardEventHandler {
 public:
  KeyboardEventHandler();
  ~KeyboardEventHandler();

  virtual void on_key_press(u8 key);
  virtual void on_key_release(u8 key);
};

class KeyboardDriver : public RinOS::Hardware::Communication::InterruptHandler,
                       public RinOS::Hardware::Driver::Driver {
  RinOS::Hardware::Communication::Port8Bit data_port;
  RinOS::Hardware::Communication::Port8Bit command_port;

  KeyboardEventHandler* event;

 public:
  KeyboardDriver(RinOS::Hardware::Communication::InterruptManager* manager,
                 KeyboardEventHandler* event);
  ~KeyboardDriver();

  virtual u32 handle_interrupt(u32 esp);

  virtual void activate();
};

}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS

#endif