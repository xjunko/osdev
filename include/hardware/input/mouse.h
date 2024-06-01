#ifndef __LOVE_HW_MOUSE_H
#define __LOVE_HW_MOUSE_H

#include <commons/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>

class MouseDriver : public InterruptHandler {
  Port8Bit data_port;
  Port8Bit command_port;

  u8 buffer[3];
  u8 offset;
  u8 buttons;

  i8 x, y;

 public:
  MouseDriver(InterruptManager* manager);
  ~MouseDriver();

  virtual u32 handle_interrupt(u32 esp);
};

#endif