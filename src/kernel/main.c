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

// will be filled by multiboot2
static u32 heap_start = 4 * 1024 * 1024;
static int total_usable_memory = 0x0;  // in bytes

void kinit_serial() { serial_init(); }

void kmb_memory(struct multiboot_tag_mmap *mmap) {
  printf("[Kernel] Memory Mapping: ");

  for (struct multiboot_mmap_entry *entry = mmap->entries;
       (u8 *)entry < (u8 *)mmap + mmap->size;
       entry =
           (struct multiboot_mmap_entry *)((u8 *)entry + mmap->entry_size)) {
    if (entry->type == 1 && entry->addr >= 0x100000) {
      u64 entry_end = entry->addr + entry->len;

      if (entry_end > heap_start) {
        u64 usable_start =
            (entry->addr < heap_start) ? heap_start : entry->addr;
        u64 usable_len = entry_end - usable_start;
        total_usable_memory += (u32)usable_len;
      }
    }
  }

  printf("Got %d mb \n", total_usable_memory / 1024 / 1024);
  printf("[Kernel] Memory limit: %d MiB\n", total_usable_memory / 1024 / 1024);
  printf("[Kernel] Heap Addr: %x\n", heap_start);
  kmemory_init(heap_start, total_usable_memory);
}

// handles memory mapping and framebuffer initialization
void kinit_multiboot(u32 mb_info) {
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_MMAP,
                         (multiboot_callback)kmb_memory);
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
  struct global_descriptor_table *gdt = new_gdt();
  if (gdt == 0) {
    printf("[Kernel] GDT creation failed\n");
    return;
  }
  printf("[Kernel] GDT created successfully\n");

  idt_init(gdt);
  idt_activate();
}

void kinit_devices() {
  // ps/2
  ps2_device_init();
  ps2_mouse_register_callback(kdebug_mouse);

  // pci
  pci_init();

  // syscalls
  // we're doing it linux style, so 0x80
  syscall_init(0x80);
}

extern int kmain(u32 mb_magic, u32 mb_info) {
  kinit_serial();
  if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("[Kernel] Invalid multiboot magic number: %x\n", mb_magic);
    while (1);  // lost cause
  }

  kinit_multiboot(mb_info);
  kinit_storage();  // ata before interrupt
  kinit_interrupts();
  kinit_devices();

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
    pit_sleep(16);
  }
}
