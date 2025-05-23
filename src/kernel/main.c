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

  struct interrupt_manager* idt = idt_init(gdt);
  if (idt == 0) {
    kprintf("[Kernel] IDT creation failed\n");
    return;
  }

  idt_activate(idt);

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
      vga_put_pixel(i, j, 0x0, 0x0, 0x0);
    }
  }
}

extern int kmain(void* multiboot_struct, u32 multiboot_magic_number) {
  kinit_serial();
  kinit_memory(multiboot_struct);
  kinit_interrupts();
  kinit_vga();

  while (1) {
    pit_sleep(100);
  }
}