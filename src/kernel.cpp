// rin_kernel.cpp - higher higher kernel
#include <commons/types.h>
#include <commons/utility.h>
#include <hardware/input/keyboard.h>
#include <hardware/input/mouse.h>
#include <hardware/interrupts.h>
#include <memory/gdt.h>
#include <terminal/term.h>

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
    RinOS::Hardware::Driver::KeyboardDriver keyboard(&interrupts);
    RinOS::Hardware::Driver::MouseDriver mouse(&interrupts);
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
