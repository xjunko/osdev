// simplified kernel, just to see what went wrong.

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/pci.h>
#include <kernel/pit.h>
#include <kernel/ps2hid.h>
#include <kernel/serial.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <kernel/vesa.h>
#include <kernel/vga.h>
#include <stdio.h>

void kinit_serial() { serial_init(); }

void kinit_memory(multiboot_info_t* mb_i) {
  u32 memupper_kb = mb_i->mem_upper;
  u32 heap_start = 4 * 1024 * 1024;

  printf("[Kernel] Memory upper: %d KB\n", memupper_kb);
  printf("[Kernel] Heap start: %x\n", heap_start);

  u32 upper_mem_bytes = memupper_kb * 1024;
  u32 free_mem_size = upper_mem_bytes - (heap_start - 0x100000) - 10 * 1024;

  printf("[Kernel] Free memory size for heap: %d bytes\n", free_mem_size);

  kmemory_init(heap_start, free_mem_size);
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

void kinit_vga(multiboot_info_t* mb_i) {
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
void sys_err(const char* msg) { asm("int $0x80" ::"a"(0x04), "b"(msg)); }

extern int kmain(void* mb_i, u32 multiboot_magic_number) {
  kinit_serial();
  kinit_memory(mb_i);
  kinit_interrupts();
  kinit_vga(mb_i);

  while (1) {
    pit_sleep(1000);
  }
}
