// rin_kernel.cpp - higher higher kernel
#include <commons/types.h>
#include <commons/utility.h>
#include <hardware/communication/interrupts.h>
#include <hardware/driver/driver.h>
#include <hardware/input/keyboard.h>
#include <hardware/input/mouse.h>
#include <memory/gdt.h>
#include <terminal/term.h>

class DumbKeyboardEventHandler
    : public RinOS::Hardware::Driver::KeyboardEventHandler {
 public:
  void on_key_press(u8 key) override {
    char* str = " ";
    str[0] = key;
    RinOS::Terminal::printf(str);
  }
};

class DumbMouseEventHandler
    : public RinOS::Hardware::Driver::MouseEventHandler {
 public:
  i8 x_, y_;

  void on_activate(i8 x, i8 y) override {
    static u16* VideoMemory = (u16*)0xb8000;
    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                              (VideoMemory[80 * y + x] & 0x0F00) << 4;

    x_ = x;
    y_ = y;
  }

  void on_move(i8 x, i8 y) override {
    static u16* VideoMemory = (u16*)0xb8000;

    VideoMemory[80 * y_ + x_] = (VideoMemory[80 * y_ + x_] & 0xF000) >> 4 |
                                (VideoMemory[80 * y_ + x_] & 0x0F00) << 4;

    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                              (VideoMemory[80 * y + x] & 0x0F00) << 4;

    x_ = x;
    y_ = y;
  }
};

class RinKernel {
 public:
  RinOS::Memory::GlobalDescriptorTable gdt;

  // Constructor
  RinKernel() {}
  ~RinKernel() {}

  // Functions
  void print_banner() {
    RinOS::Terminal::set_color(0x0F);
    RinOS::Terminal::clear();
    RinOS::Terminal::set_position(0, 0);

    RinOS::Terminal::set_color(0xF0);
    RinOS::Terminal::printf("RinOS v0.0.1");

    for (u8 i = 12; i < 80; i++) {
      RinOS::Terminal::printf(" ");
    }

    RinOS::Terminal::set_color(0x0F);
    RinOS::Terminal::set_position(0, 24);
    RinOS::Terminal::printf("Press any key to continue!\n");
  }

  // Entry
  void initialize() {
    RinOS::Memory::GlobalDescriptorTable gdt;
    RinOS::Hardware::Communication::InterruptManager interrupts(&gdt);

    // Drivers
    RinOS::Hardware::Driver::DriverManager driver_manager;

    DumbKeyboardEventHandler kb_event;
    RinOS::Hardware::Driver::KeyboardDriver keyboard(&interrupts, &kb_event);

    DumbMouseEventHandler mouse_event;
    RinOS::Hardware::Driver::MouseDriver mouse(&interrupts, &mouse_event);

    driver_manager.add_driver(&keyboard);
    driver_manager.add_driver(&mouse);

    driver_manager.activate_all();
    interrupts.activate();

    // Fake loading...
    RinOS::Terminal::log("System", "Booting!");
    RinOS::Utility::sleep(5000);

    // Terminal UI
    print_banner();

    while (true) {
    }
  }
};
