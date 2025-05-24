// simplified kernel, just to see what went wrong.

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pci.h>
#include <kernel/pit.h>
#include <kernel/ps2hid.h>
#include <kernel/serial.h>
#include <kernel/sys/syscalls.h>
#include <kernel/types.h>
#include <kernel/vga.h>
#include <malloc.h>
#include <stdio.h>

void kinit_serial() { serial_init(); }

void kinit_memory(void* multiboot_struct) {
  u32* memupper = (u32*)(((u32)multiboot_struct) + 8);
  u32 heap = 4 * 1024 * 1024;

  printf("[Kernel] Memory upper: %d\n", *memupper);
  printf("[Kernel] Heap: %d\n", heap);
  printf("[Kernel] Heap end: %d\n", *memupper + heap);
  printf("[Kernel] Heap end: %d\n", *memupper + heap);
  printf("[Kernel] Heap end: %d\n", *memupper + heap);

  _init_memory(heap, (*memupper) * 1024 - heap - 10 * 1024);
}

void kdebug_mouse(struct ps2_mouse_state state) {
  printf("[Mouse] x: %d, y: %d, buttons: %d %d %d\n", state.x, state.y,
         state.buttons[0], state.buttons[1], state.buttons[2]);

  for (int i = 0; i < 320; i++) {
    for (int j = 0; j < 200; j++) {
      vga_put_pixel(i, j, 0x0, 0x0, 0x0);
    }
  }
  vga_put_pixel(state.x, state.y, 0xFF, 0xFF, 0xFF);
  vga_draw();
}

void kinit_interrupts() {
  struct global_descriptor_table* gdt = new_gdt();
  if (gdt == 0) {
    printf("[Kernel] GDT creation failed\n");
    return;
  }
  printf("[Kernel] GDT created successfully\n");

  idt_init(gdt);
  idt_activate();

  // ps/2
  ps2_device_init();
  ps2_mouse_register_callback(kdebug_mouse);

  // pci
  pci_init();

  // syscalls
  // we're doing it linux style, so 0x80
  syscall_init(0x80);
}

void kinit_vga() {
  vga_init();
  vga_set_mode(640, 480, 32);

  // pallete test
  for (int y = 0; y < 200; y++) {
    for (int x = 0; x < 320; x++) {
      u8 r = x % 255;
      u8 g = y % 255;
      u8 b = (x + y) % 255;

      vga_put_pixel(x, y, r, g, b);
    }
  }

  vga_draw();
}

// syscall test
// void sys_err(const char* msg) { asm("int $0x80" ::"a"(0x04), "b"(msg)); }

extern int kmain(void* multiboot_struct, u32 multiboot_magic_number) {
  kinit_serial();
  kinit_memory(multiboot_struct);
  kinit_interrupts();
  kinit_vga();

  while (1) {
    pit_sleep(1000);
  }
}