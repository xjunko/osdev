// simplified kernel, just to see what went wrong.

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/misc/kprintf.h>
#include <kernel/pci.h>
#include <kernel/pit.h>
#include <kernel/ps2hid.h>
#include <kernel/types.h>
#include <kernel/vga.h>
#include <stdlib/malloc.h>

void kinit_serial() { _init_dev_serial(); }

void kinit_memory(void* multiboot_struct) {
  u32* memupper = (u32*)(((u32)multiboot_struct) + 8);
  u32 heap = 4 * 1024 * 1024;

  kprintf("[Kernel] Memory upper: %d\n", *memupper);
  kprintf("[Kernel] Heap: %d\n", heap);
  kprintf("[Kernel] Heap end: %d\n", *memupper + heap);
  kprintf("[Kernel] Heap end: %d\n", *memupper + heap);
  kprintf("[Kernel] Heap end: %d\n", *memupper + heap);

  _init_memory(heap, (*memupper) * 1024 - heap - 10 * 1024);
}

void kinit_interrupts() {
  struct global_descriptor_table* gdt = new_gdt();
  if (gdt == 0) {
    kprintf("[Kernel] GDT creation failed\n");
    return;
  }
  kprintf("[Kernel] GDT created successfully\n");

  struct interrupt_manager* idt = new_idt(gdt);
  if (idt == 0) {
    kprintf("[Kernel] IDT creation failed\n");
    return;
  }

  interrupt_manager_active(idt);

  // ps/2
  ps2_kb_init(idt);

  // pci
  pci_select_drivers(idt);
}

void kinit_vga() {
  vga_init();
  vga_set_mode(640, 480, 32);

  for (int i = 0; i < 320; i++) {
    for (int j = 0; j < 200; j++) {
      vga_put_pixel(i, j, 0xFF, 0xFF, 0xFF);
    }
  }
}

extern int kmain(void* multiboot_struct, u32 multiboot_magic_number) {
  kinit_serial();
  kinit_memory(multiboot_struct);
  kinit_interrupts();
  kinit_vga();

  u32 c = 0x0;

  while (1) {
    pit_sleep(100);
    for (int i = 0; i < 320; i++) {
      for (int j = 0; j < 200; j++) {
        u8 r = (u8)((i + c) % 256);
        u8 g = (u8)((j + c) % 256);
        u8 b = (u8)((i + j + c) % 256);
        vga_put_pixel(i, j, r, g, b);
      }
    }
    c += 32;
  }
}