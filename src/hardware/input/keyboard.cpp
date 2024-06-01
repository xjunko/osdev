#include <hardware/input/keyboard.h>
#include <terminal/term.h>

using namespace RinOS::Hardware::Driver;

KeyboardDriver::KeyboardDriver(
    RinOS::Hardware::Communication::InterruptManager* manager)
    : InterruptHandler(0x21, manager), data_port(0x60), command_port(0x64) {
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

KeyboardDriver::~KeyboardDriver() {}
u32 KeyboardDriver::handle_interrupt(u32 esp) {
  u8 key = data_port.Read();

  // Only key press.
  if (0 < 0x80) {
    switch (key) {
      // special chars 1
      case 0x29:
        RinOS::Terminal::printf("`");
        break;
      case 0x02:
        RinOS::Terminal::printf("1");
        break;
      case 0x03:
        RinOS::Terminal::printf("2");
        break;
      case 0x04:
        RinOS::Terminal::printf("3");
        break;
      case 0x05:
        RinOS::Terminal::printf("4");
        break;
      case 0x06:
        RinOS::Terminal::printf("5");
        break;
      case 0x07:
        RinOS::Terminal::printf("6");
        break;
      case 0x08:
        RinOS::Terminal::printf("7");
        break;
      case 0x09:
        RinOS::Terminal::printf("8");
        break;
      case 0x0a:
        RinOS::Terminal::printf("9");
        break;
      case 0x0b:
        RinOS::Terminal::printf("0");
        break;
      // special chars 2
      case 0x0c:
        RinOS::Terminal::printf("-");
        break;
      case 0x0d:
        RinOS::Terminal::printf("=");
        break;
      // first row of letters
      case 0x10:
        RinOS::Terminal::printf("q");
        break;
      case 0x11:
        RinOS::Terminal::printf("w");
        break;
      case 0x12:
        RinOS::Terminal::printf("e");
        break;
      case 0x13:
        RinOS::Terminal::printf("r");
        break;
      case 0x14:
        RinOS::Terminal::printf("t");
        break;
      case 0x15:
        RinOS::Terminal::printf("y");
        break;
      case 0x16:
        RinOS::Terminal::printf("u");
        break;
      case 0x17:
        RinOS::Terminal::printf("i");
        break;
      case 0x18:
        RinOS::Terminal::printf("o");
        break;
      case 0x19:
        RinOS::Terminal::printf("p");
        break;
      // special chars 3
      case 0x1a:
        RinOS::Terminal::printf("[");
        break;
      case 0x1b:
        RinOS::Terminal::printf("]");
        break;
      case 0x2b:
        RinOS::Terminal::printf("\\");
        break;
      // second row of letters
      case 0x1e:
        RinOS::Terminal::printf("a");
        break;
      case 0x1f:
        RinOS::Terminal::printf("s");
        break;
      case 0x20:
        RinOS::Terminal::printf("d");
        break;
      case 0x21:
        RinOS::Terminal::printf("f");
        break;
      case 0x22:
        RinOS::Terminal::printf("g");
        break;
      case 0x23:
        RinOS::Terminal::printf("h");
        break;
      case 0x24:
        RinOS::Terminal::printf("j");
        break;
      case 0x25:
        RinOS::Terminal::printf("k");
        break;
      case 0x26:
        RinOS::Terminal::printf("l");
        break;
      // special chars 4
      case 0x27:
        RinOS::Terminal::printf(";");
        break;
      case 0x28:
        RinOS::Terminal::printf("'");
        break;
      // third row of letters
      case 0x2c:
        RinOS::Terminal::printf("z");
        break;
      case 0x2d:
        RinOS::Terminal::printf("x");
        break;
      case 0x2e:
        RinOS::Terminal::printf("c");
        break;
      case 0x2f:
        RinOS::Terminal::printf("v");
        break;
      case 0x30:
        RinOS::Terminal::printf("b");
        break;
      case 0x31:
        RinOS::Terminal::printf("n");
        break;
      case 0x32:
        RinOS::Terminal::printf("m");
        break;
      // special chars 5
      case 0x33:
        RinOS::Terminal::printf(",");
        break;
      case 0x34:
        RinOS::Terminal::printf(".");
        break;
      case 0x35:
        RinOS::Terminal::printf("/");
        break;

      // space
      case 0x39:
        RinOS::Terminal::printf(" ");
        break;
      // enter
      case 0x1c:
        RinOS::Terminal::printf("\n");
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
        RinOS::Terminal::printf("\b");
        break;
      // escape
      case 0x01:
        RinOS::Terminal::printf("\x1b");
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
        RinOS::Terminal::printf("\v");
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
        RinOS::Terminal::printf("\xfc");
        break;
      case 0x48:
        RinOS::Terminal::printf("\xfd");
        break;
      case 0x50:
        RinOS::Terminal::printf("\xfe");
        break;
      case 0x4d:
        RinOS::Terminal::printf("\xff");
        break;
    }
  }

  return esp;
}
