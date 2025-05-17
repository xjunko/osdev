#ifndef __LOVE_PIT_TIMER_H
#define __LOVE_PIT_TIMER_H

#include <commons/types.h>
#include <kernel/hardware/communication/interrupts.h>
#include <kernel/hardware/communication/port.h>
#include <kernel/hardware/driver/driver.h>

namespace RinOS {
namespace Hardware {
namespace Timer {
class BasicPIT : RinOS::Hardware::Driver::Driver {
  RinOS::Hardware::Communication::Port8Bit channel_0;
  RinOS::Hardware::Communication::Port8Bit channel_1;
  RinOS::Hardware::Communication::Port8Bit channel_2;
  RinOS::Hardware::Communication::Port8Bit command_port;

 public:
  BasicPIT();
  ~BasicPIT();

  virtual u32 read_count();
  virtual void set_count(u32 count);
};

}  // namespace Timer
};  // namespace Hardware
};  // namespace RinOS

#endif