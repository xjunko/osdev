#ifndef __LOVE_HW_MOUSE_H
#define __LOVE_HW_MOUSE_H

#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/port.h>
#include <hardware/driver/driver.h>

namespace RinOS {
namespace Hardware {
namespace Driver {
class MouseEventHandler {
 public:
  MouseEventHandler();
  ~MouseEventHandler();

  virtual void on_activate(i8 x, i8 y);
  virtual void on_move(i8 x, i8 y);
  virtual void on_mouse_press(u8 button, u8 state);
};

class MouseDriver : public RinOS::Hardware::Communication::InterruptHandler,
                    public RinOS::Hardware::Driver::Driver {
  RinOS::Hardware::Communication::Port8Bit data_port;
  RinOS::Hardware::Communication::Port8Bit command_port;

  u8 buffer[3];
  u8 offset;
  u8 buttons;

  i8 x, y;

  MouseEventHandler* event;

 public:
  MouseDriver(RinOS::Hardware::Communication::InterruptManager* manager,
              MouseEventHandler* event);
  ~MouseDriver();

  virtual u32 handle_interrupt(u32 esp);

  virtual void activate();
};
}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS
#endif