#include <kernel/hardware/input/mouse.h>
#include <kernel/terminal/term.h>

using namespace RinOS::Hardware::Driver;

MouseEventHandler::MouseEventHandler() {}

void MouseEventHandler::on_activate(i8 x, i8 y) {}
void MouseEventHandler::on_move(i8 x, i8 y) {}
void MouseEventHandler::on_mouse_press(u8 button, u8 state) {}

MouseDriver::MouseDriver(
    RinOS::Hardware::Communication::InterruptManager* manager,
    MouseEventHandler* event)
    : InterruptHandler(0x2C, manager), data_port(0x60), command_port(0x64) {
  this->event = event;
}

MouseDriver::~MouseDriver() {}

void MouseDriver::activate() {
  offset = 0;
  buttons = 0;
  x = 40;
  y = 12;

  command_port.Write(0xA8);
  command_port.Write(0x20);

  u8 status = data_port.Read() | 2;
  command_port.Write(0x60);
  data_port.Write(status);

  command_port.Write(0xD4);
  data_port.Write(0xF4);
  data_port.Read();

  event->on_activate(x, y);
}

u32 MouseDriver::handle_interrupt(u32 esp) {
  u8 status = command_port.Read();

  if (!(status & 0x20)) {
    return esp;
  }

  buffer[offset] = data_port.Read();

  if (event == 0) {
    return esp;
  }

  offset = (offset + 1) % 3;

  if (offset == 0) {
    x += buffer[1];
    y -= buffer[2];

    if (x < 0) x = 0;
    if (x >= 80) x = 79;
    if (y < 0) y = 0;
    if (y >= 25) y = 24;

    event->on_move(x, y);
  }

  return esp;
}
