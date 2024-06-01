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
  Memory::GlobalDescriptorTable gdt;

  // Constructor
  RinKernel() {}
  ~RinKernel() {}

  // Methods
  void initialize() {
    Terminal::set_color(0x0E);
    Terminal::printf("Operating System Version 0.0.1 \n");

    Terminal::set_color(0x0A);

    Terminal::printf("Initializing GDT... \n");
    Memory::GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);
    interrupts.activate();

    // TODO:
    // - IRQs
    // - PIT
    // - Mouse

    // // Print these TODOs in a for loop
    Terminal::set_color(0x04);
    Utility::wait_for_a_bit(25);
    Terminal::printf("TODO: Implement IRQs \n");
    Utility::wait_for_a_bit(25);
    Terminal::printf("TODO: Implement PIT \n");
    Utility::wait_for_a_bit(25);
    Terminal::printf("TODO: Implement Mouse \n");

    // Fake ass terminal prompt
    Terminal::set_color(0x0F);
    Terminal::printf("\n[junko@konno] $ ");

    while (true) {
    }
  }
};
