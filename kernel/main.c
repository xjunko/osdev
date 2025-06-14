// simplified kernel, just to see what went wrong.

#include <kernel/framebuffer.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kmalloc.h>
#include <kernel/kprintf.h>
#include <kernel/multiboot.h>
#include <kernel/pci.h>
#include <kernel/pit.h>
#include <kernel/ps2hid.h>
#include <kernel/serial.h>
#include <kernel/syscall.h>
#include <kernel/tasks.h>
#include <kernel/tty.h>
#include <kernel/types.h>
#include <kernel/vfs.h>
#include <stdio.h>
#include <unistd.h>

void kinit_serial() { serial_init(); }

// sets up the kernel internal memory (to be removed)
// and framebuffer
void kinit_multiboot_bind() {
  kprintf("[Kernel] Initiating Multiboot\n");
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_MMAP,
                         (multiboot_callback)kmalloc_init);
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_FRAMEBUFFER,
                         (multiboot_callback)framebuffer_init);
}

void kinit_multiboot_dispatch(uint32_t mb_info) {
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

void _keyboard_input(struct ps2_kb_state state) {}

void kmain_loop() {
  ps2_kb_register_callback(_keyboard_input);

  tty_init();
  tty_write("[kernel] kernel started! \n");

  char buf[256];
  scanf("%s", buf);
  printf("[kernel] scanf returned: %s\n", buf);

  tty_write("rin@root: ");

  while (1) {
    asm volatile("hlt");
  }
}

extern int kmain(uint32_t mb_magic, uint32_t mb_info) {
  kinit_serial();
  if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    kprintf("[Kernel] Invalid multiboot magic number: %x\n", mb_magic);
    while (1);  // lost cause
  }

  kinit_multiboot_bind();
  kinit_interrupts();
  kinit_multiboot_dispatch(mb_info);
  kprintf("[Kernel] Booted into kernel! \n");

  // userspace, libc should be available now
  kinit_storage();
  kinit_devices();

  // run the main loop in a task
  struct cpu_task* task = cpu_task_new(kmain_loop);
  if (task == 0) {
    kprintf("[Kernel] Failed to create main loop task\n");
    asm("hlt");
  }
  cpu_task_add(task);
  pit_write(PIT_HZ / PIT_SCALE);
  while (1) {
    asm("hlt");
  }
}
