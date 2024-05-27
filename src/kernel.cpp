// kernel.cpp - high level kernel
#include <commons/types.h>
#include <constructors.h>
#include <memory/gdt.h>
#include <terminal/term.h>

#include "rin_kernel.cpp"

void wait_for_a_bit(int how_long) {
  for (int i = 0; i < how_long; i++) {
    for (int j = 0; j < 1000000; j++) {
      __asm__("nop");
    }
  }
}

extern "C" void kernel_entry(void *multiboot_struct, u32 magic_number) {
  RinKernel kernel = {};
  kernel.initialize();
}
