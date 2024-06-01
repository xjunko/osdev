#include <hardware/input/mouse.h>
#include <terminal/term.h>

MouseDriver::MouseDriver(InterruptManager* manager)
    : InterruptHandler(0x2C, manager), data_port(0x60), command_port(0x64) {
  offset = 0;
  buttons = 0;
  x = 40;
  y = 12;

  u16* VideoMemory = (u16*)0xb8000;

  VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 |
                            (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                            (VideoMemory[80 * y + x] & 0x00FF);

  command_port.Write(0xA8);  // Activate interrupts
  command_port.Write(0x20);  // Command 0x20 = Read Controller Command Byte

  u8 status = data_port.Read() | 2;
  command_port.Write(0x60);  // Command 0x60 = Set Controller Command Byte
  data_port.Write(status);

  command_port.Write(0xD4);
  data_port.Write(0xF4);
  data_port.Read();
}

MouseDriver::~MouseDriver() {}

u32 MouseDriver::handle_interrupt(u32 esp) {
  u8 status = command_port.Read();

  if (!(status & 0x20)) {
    return esp;
  }

  buffer[offset] = data_port.Read();
  offset = (offset + 1) % 3;

  if (offset == 0) {
    static u8 x = 40, y = 12;

    static u16* VideoMemory = (u16*)0xb8000;

    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                              (VideoMemory[80 * y + x] & 0x0F00) << 4;

    x += buffer[1];
    y -= buffer[2];

    if (x < 0) x = 0;
    if (x >= 80) x = 79;
    if (y < 0) y = 0;
    if (y >= 25) y = 24;

    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                              (VideoMemory[80 * y + x] & 0x0F00) << 4;

    // for (u8 i = 0; i < 3; i++) {
    //   if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))) {
    //     // Mouse Press
    //   }
    // }

    // buttons = buffer[0];
  }

  return esp;
}
