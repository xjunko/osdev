#include <hardware/input/keyboard.h>
#include <terminal/term.h>

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
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
        Terminal::printf("`");
        break;
      case 0x02:
        Terminal::printf("1");
        break;
      case 0x03:
        Terminal::printf("2");
        break;
      case 0x04:
        Terminal::printf("3");
        break;
      case 0x05:
        Terminal::printf("4");
        break;
      case 0x06:
        Terminal::printf("5");
        break;
      case 0x07:
        Terminal::printf("6");
        break;
      case 0x08:
        Terminal::printf("7");
        break;
      case 0x09:
        Terminal::printf("8");
        break;
      case 0x0a:
        Terminal::printf("9");
        break;
      case 0x0b:
        Terminal::printf("0");
        break;
      // special chars 2
      case 0x0c:
        Terminal::printf("-");
        break;
      case 0x0d:
        Terminal::printf("=");
        break;
      // first row of letters
      case 0x10:
        Terminal::printf("q");
        break;
      case 0x11:
        Terminal::printf("w");
        break;
      case 0x12:
        Terminal::printf("e");
        break;
      case 0x13:
        Terminal::printf("r");
        break;
      case 0x14:
        Terminal::printf("t");
        break;
      case 0x15:
        Terminal::printf("y");
        break;
      case 0x16:
        Terminal::printf("u");
        break;
      case 0x17:
        Terminal::printf("i");
        break;
      case 0x18:
        Terminal::printf("o");
        break;
      case 0x19:
        Terminal::printf("p");
        break;
      // special chars 3
      case 0x1a:
        Terminal::printf("[");
        break;
      case 0x1b:
        Terminal::printf("]");
        break;
      case 0x2b:
        Terminal::printf("\\");
        break;
      // second row of letters
      case 0x1e:
        Terminal::printf("a");
        break;
      case 0x1f:
        Terminal::printf("s");
        break;
      case 0x20:
        Terminal::printf("d");
        break;
      case 0x21:
        Terminal::printf("f");
        break;
      case 0x22:
        Terminal::printf("g");
        break;
      case 0x23:
        Terminal::printf("h");
        break;
      case 0x24:
        Terminal::printf("j");
        break;
      case 0x25:
        Terminal::printf("k");
        break;
      case 0x26:
        Terminal::printf("l");
        break;
      // special chars 4
      case 0x27:
        Terminal::printf(";");
        break;
      case 0x28:
        Terminal::printf("'");
        break;
      // third row of letters
      case 0x2c:
        Terminal::printf("z");
        break;
      case 0x2d:
        Terminal::printf("x");
        break;
      case 0x2e:
        Terminal::printf("c");
        break;
      case 0x2f:
        Terminal::printf("v");
        break;
      case 0x30:
        Terminal::printf("b");
        break;
      case 0x31:
        Terminal::printf("n");
        break;
      case 0x32:
        Terminal::printf("m");
        break;
      // special chars 5
      case 0x33:
        Terminal::printf(",");
        break;
      case 0x34:
        Terminal::printf(".");
        break;
      case 0x35:
        Terminal::printf("/");
        break;

      // space
      case 0x39:
        Terminal::printf(" ");
        break;
      // enter
      case 0x1c:
        Terminal::printf("\n");
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
        Terminal::printf("\b");
        break;
      // escape
      case 0x01:
        Terminal::printf("\x1b");
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
        Terminal::printf("\v");
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
        Terminal::printf("\xfc");
        break;
      case 0x48:
        Terminal::printf("\xfd");
        break;
      case 0x50:
        Terminal::printf("\xfe");
        break;
      case 0x4d:
        Terminal::printf("\xff");
        break;
    }
  }

  return esp;
}
