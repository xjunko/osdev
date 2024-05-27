// kernel.cpp - high level kernel
#include <commons/types.h>
#include <constructors.h>
#include <memory/gdt.h>
#include <terminal/term.h>

void wait_for_a_bit(int how_long) {
  for (int i = 0; i < how_long; i++) {
    for (int j = 0; j < 1000000; j++) {
      __asm__("nop");
    }
  }
}

extern "C" void kernel_entry(void *multiboot_struct, u32 magic_number) {
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
