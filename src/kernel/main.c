// simplified kernel, just to see what went wrong.

#include <kernel/ata.h>
#include <kernel/dos.h>
#include <kernel/framebuffer.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory.h>
#include <kernel/mikufs.h>
#include <kernel/multiboot.h>
#include <kernel/pci.h>
#include <kernel/pit.h>
#include <kernel/ps2hid.h>
#include <kernel/serial.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>
#include <unistd.h>

void kinit_serial() { serial_init(); }

// sets up the kernel internal memory (to be removed)
void kinit_multiboot_stage1(u32 mb_info) {
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_MMAP,
                         (multiboot_callback)kmalloc_init);
  multiboot_start(mb_info);
}

// sets up the framebuffer0
void kinit_multiboot_stage2(u32 mb_info) {
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_FRAMEBUFFER,
                         (multiboot_callback)framebuffer_init);
  multiboot_start(mb_info);
}

void kdebug_mouse(struct ps2_mouse_state state) {
#ifdef KERNEL_DEBUG
  printf("[Mouse] x: %d, y: %d, buttons: %d %d %d\n", state.x, state.y,
         state.buttons[0], state.buttons[1], state.buttons[2]);
#endif
  framebuffer_fill_rect(state.x, state.y, 16, 16, 255, 0, 0);
  framebuffer_flush();
}

void kinit_storage() {
  struct ata_device *master = ata_new(true, 0x1F0);
  ata_identify(master);

  struct ata_device *slave = ata_new(false, 0x1F0);
  ata_identify(slave);

  mikufs_init(slave);

  // FILE *file = fopen("hello.txt", "r");
  // fclose(file);
  // mikufs_write(slave, "hello.txt", (u8 *)"world!", 7);
  // mikufs_write(slave, "giga.txt", (u8 *)"chad", 5);
  // u8 *data = mikufs_read(slave, "hello.txt");
  // free(data);
  // mikufs_read(slave);
}

void kinit_interrupts() {
  gdt_init();
  idt_init();
  syscall_init(0x60);
  idt_activate();
}

void kinit_devices() {
  // ps/2
  ps2_device_init();
  ps2_mouse_register_callback(kdebug_mouse);

  // pci
  pci_init();
}

extern int kmain(u32 mb_magic, u32 mb_info) {
  kinit_serial();
  if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("[Kernel] Invalid multiboot magic number: %x\n", mb_magic);
    while (1);  // lost cause
  }
  kinit_multiboot_stage1(mb_info);
  // kinit_storage();
  kinit_interrupts();
  kinit_multiboot_stage2(mb_info);

  kinit_devices();

  int r = 0;
  int g = 0;
  int b = 0;
  int dr = 1, dg = 2, db = 3;

  FILE *f = fopen("/dev/t0", "r");
  fclose(f);
  void *memtest = malloc(4096);
  // write(1, "Hello\n", 6);
  // _exit(0);

  while (1) {
    r = (r + dr) % 256;
    g = (g + dg) % 256;
    b = (b + db) % 256;

    framebuffer_clear(r, g, b);
    framebuffer_flush();
    pit_sleep(16);
  }
}
