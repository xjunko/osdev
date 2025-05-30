// simplified kernel, just to see what went wrong.

#include <kernel/framebuffer.h>
#include <kernel/fs/vfs.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory.h>
#include <kernel/misc/kio.h>
#include <kernel/multiboot.h>
#include <kernel/multitask.h>
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
// and framebuffer
void kinit_multiboot_bind(u32 mb_info) {
  kprintf("[Kernel] Initiating Multiboot\n");
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_MMAP,
                         (multiboot_callback)kmalloc_init);
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_FRAMEBUFFER,
                         (multiboot_callback)framebuffer_init);
}

void kinit_multiboot_dispatch(u32 mb_info) {
  multiboot_start(mb_info);
  kprintf("[Kernel] Multiboot, dispatched! \n");
}

void kdebug_mouse(struct ps2_mouse_state state) {
#ifdef KERNEL_DEBUG
  printf("[Mouse] x: %d, y: %d, buttons: %d %d %d\n", state.x, state.y,
         state.buttons[0], state.buttons[1], state.buttons[2]);
#endif
  framebuffer_fill_rect(state.x, state.y, 16, 16, 255, 0, 0);
  framebuffer_flush();
}

void kinit_storage() { vfs_init(); }

void kinit_interrupts() {
  gdt_init();
  idt_init();
  syscall_init();
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
    kprintf("[Kernel] Invalid multiboot magic number: %x\n", mb_magic);
    while (1);  // lost cause
  }

  kinit_multiboot_bind(mb_info);
  kinit_interrupts();
  kinit_multiboot_dispatch(mb_info);
  kprintf("[Kernel] Booted into kernel! \n");

  // userspace, libc should be available now
  kinit_storage();
  kinit_devices();

  // rgb weee :D
  int r = 0;
  int g = 0;
  int b = 0;
  int dr = 1, dg = 2, db = 3;

  while (1) {
    r = (r + dr) % 256;
    g = (g + dg) % 256;
    b = (b + db) % 256;

    framebuffer_clear(r, g, b);
    framebuffer_flush();
    pit_sleep(100);  // sleep for 100ms
  }
}
