// entry.cpp - entry point for the kernel
#include <commons/types.h>
#include <constructors.h>
#include <kernel/memory/gdt.h>
#include <kernel/terminal/term.h>

#include "kernel.cpp"

extern "C" void kernel_entry(void *multiboot_struct, u32 magic_number) {
  RinKernel kernel = {};
  kernel.initialize();
}
