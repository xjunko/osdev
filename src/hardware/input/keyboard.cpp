#include <hardware/input/keyboard.h>
#include <terminal/term.h>

using namespace RinOS::Hardware::Driver;

KeyboardEventHandler::KeyboardEventHandler() {}

void KeyboardEventHandler::on_key_press(u8 key) {}

void KeyboardEventHandler::on_key_release(u8 key) {}

KeyboardDriver::KeyboardDriver(
    RinOS::Hardware::Communication::InterruptManager* manager,
    KeyboardEventHandler* event)
    : InterruptHandler(0x21, manager), data_port(0x60), command_port(0x64) {
  this->event = event;
}

KeyboardDriver::~KeyboardDriver() {}

void KeyboardDriver::activate() {
  while (command_port.Read() & 0x1) {
    data_port.Read();
  }

  command_port.Write(0xAE);  // Activate interrupts
  command_port.Write(0x20);  // Command 0x20 = Read Controller Command Byte
  u8 status = (data_port.Read() | 1) & ~0x10;
  command_port.Write(0x60);  // Command 0x60 = Set Controller Command Byte
  data_port.Write(status);
  data_port.Write(0xF4);
}

u32 KeyboardDriver::handle_interrupt(u32 esp) {
  u8 key = data_port.Read();

  if (event == 0) {
    return esp;
  }

  // Only key press.
  if (0 < 0x80) {
    switch (key) {
      // special chars 1
      case 0x29:
        event->on_key_press('`');
        break;
      case 0x02:
        event->on_key_press('1');
        break;
      case 0x03:
        event->on_key_press('2');
        break;
      case 0x04:
        event->on_key_press('3');
        break;
      case 0x05:
        event->on_key_press('4');
        break;
      case 0x06:
        event->on_key_press('5');
        break;
      case 0x07:
        event->on_key_press('6');
        break;
      case 0x08:
        event->on_key_press('7');
        break;
      case 0x09:
        event->on_key_press('8');
        break;
      case 0x0a:
        event->on_key_press('9');
        break;
      case 0x0b:
        event->on_key_press('0');
        break;
      // special chars 2
      case 0x0c:
        event->on_key_press('-');
        break;
      case 0x0d:
        event->on_key_press('=');
        break;
      // first row of letters
      case 0x10:
        event->on_key_press('q');
        break;
      case 0x11:
        event->on_key_press('w');
        break;
      case 0x12:
        event->on_key_press('e');
        break;
      case 0x13:
        event->on_key_press('r');
        break;
      case 0x14:
        event->on_key_press('t');
        break;
      case 0x15:
        event->on_key_press('y');
        break;
      case 0x16:
        event->on_key_press('u');
        break;
      case 0x17:
        event->on_key_press('i');
        break;
      case 0x18:
        event->on_key_press('o');
        break;
      case 0x19:
        event->on_key_press('p');
        break;
      // special chars 3
      case 0x1a:
        event->on_key_press('[');
        break;
      case 0x1b:
        event->on_key_press(']');
        break;
      case 0x2b:
        event->on_key_press('\\');
        break;
      // second row of letters
      case 0x1e:
        event->on_key_press('a');
        break;
      case 0x1f:
        event->on_key_press('s');
        break;
      case 0x20:
        event->on_key_press('d');
        break;
      case 0x21:
        event->on_key_press('f');
        break;
      case 0x22:
        event->on_key_press('g');
        break;
      case 0x23:
        event->on_key_press('h');
        break;
      case 0x24:
        event->on_key_press('j');
        break;
      case 0x25:
        event->on_key_press('k');
        break;
      case 0x26:
        event->on_key_press('l');
        break;
      // special chars 4
      case 0x27:
        event->on_key_press(';');
        break;
      case 0x28:
        event->on_key_press('\'');
        break;
      // third row of letters
      case 0x2c:
        event->on_key_press('z');
        break;
      case 0x2d:
        event->on_key_press('x');
        break;
      case 0x2e:
        event->on_key_press('c');
        break;
      case 0x2f:
        event->on_key_press('v');
        break;
      case 0x30:
        event->on_key_press('b');
        break;
      case 0x31:
        event->on_key_press('n');
        break;
      case 0x32:
        event->on_key_press('m');
        break;
      // special chars 5
      case 0x33:
        event->on_key_press(',');
        break;
      case 0x34:
        event->on_key_press('.');
        break;
      case 0x35:
        event->on_key_press('/');
        break;

      // space
      case 0x39:
        event->on_key_press(' ');
        break;
      // enter
      case 0x1c:
        event->on_key_press('\n');
        break;
      // caps lock
      case 0x3a:
        // Do something with caps lock if necessary
        break;
      // windows key
      case 0x5b:
        break;

      // backspace
      case 0x0e:
        event->on_key_press('\b');
        break;
      // escape
      case 0x01:
        event->on_key_press('\x1b');
        break;

      // right control
      case 0x1d:
        // Handle right control if necessary
        break;
      case 0x9d:
        // Handle right control if necessary
        break;

      // tab
      case 0x0f:
        event->on_key_press('\v');
        break;

      // alt keys
      case 0x38:
        // Handle alt key if necessary
        break;
      case 0xb8:
        // Handle alt key if necessary
        break;
      case 0xe0:
        // Handle alt key if necessary
        break;

      // f# keys
      case 0x3b:
        // Handle F# keys if necessary
        break;
      case 0xbb:
        // Handle F# keys if necessary
        break;

      // arrow keys
      case 0x4b:
        event->on_key_press('\xfc');
        break;
      case 0x48:
        event->on_key_press('\xfd');
        break;
      case 0x50:
        event->on_key_press('\xfe');
        break;
      case 0x4d:
        event->on_key_press('\xff');
        break;
    }
  }

  return esp;
}
