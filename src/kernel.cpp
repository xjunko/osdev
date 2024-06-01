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
  DumbKeyboardEventHandler() {}
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
  DumbMouseEventHandler() {}
  void on_activate(i8 x, i8 y) override {
    static u16* VideoMemory = (u16*)0xb8000;
    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                              (VideoMemory[80 * y + x] & 0x0F00) << 4;
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

  // Methods
  void initialize() {
    RinOS::Terminal::set_color(0x0E);
    RinOS::Terminal::printf("Operating System Version 0.0.1 \n");
    RinOS::Terminal::set_color(0x0F);

    RinOS::Terminal::set_color(0x0A);
    RinOS::Terminal::printf("Initializing GDT... \n");
    RinOS::Terminal::set_color(0x0F);

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

    // TODO:
    // - IRQs
    // - PIT

    // // Print these TODOs in a for loop
    // RinOS::Terminal::set_color(0x04);
    RinOS::Terminal::printf("TODO: Implement IRQs \n");
    RinOS::Terminal::printf("TODO: Implement PIT \n");

    // Fake ass terminal prompt
    // RinOS::Terminal::set_color(0x0F);
    RinOS::Terminal::printf("\n[junko@konno] $ ");

    while (true) {
    }
  }
};
