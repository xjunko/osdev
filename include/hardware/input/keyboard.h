#ifndef __LOVE_HW_KEYBOARD_H
#define __LOVE_HW_KEYBOARD_H

#include <commons/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>

class KeyboardDriver : public InterruptHandler {
  Port8Bit data_port;
  Port8Bit command_port;

 public:
  KeyboardDriver(InterruptManager* manager);
  ~KeyboardDriver();

  virtual u32 handle_interrupt(u32 esp);
};

#endif