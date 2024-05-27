// rin_kernel.cpp - higher higher kernel
#include <commons/types.h>
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
    Terminal::printf("Kagamine Rin Operating System Version 0.0.1 \n");

    Terminal::set_color(0x0A);
    Terminal::printf("Initializing GDT... \n");
    Memory::GlobalDescriptorTable gdt;

    // Print for loop for fun
    Terminal::set_color(0x0F);
    Terminal::printf("\n");
    for (int i = 0; i < 10; i++) {
      Terminal::printf("Hello, world! \n");
    }

    while (true) {
    }
  }
};
