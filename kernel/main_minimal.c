// simplified kernel, just to see what went wrong.

#include <kernel/gdt2.h>
#include <kernel/idt2.h>
#include <kernel/irq.h>
#include <kernel/isr.h>
#include <kernel/kprintf.h>
#include <kernel/multiboot.h>
#include <kernel/serial.h>
#include <kernel/types.h>

#define KERNEL_MINIMAL 1

extern char _bss_end;
static void __multiboot_kmalloc(struct multiboot_tag_mmap* mmap) {
  uintptr_t kernel_end = ALIGN_UP((uintptr_t)&_bss_end, 0x1000);
  uintptr_t heap_base = 0;

  for (struct multiboot_mmap_entry* entry = mmap->entries;
       (uintptr_t)entry < (uintptr_t)mmap + mmap->size;
       entry = (struct multiboot_mmap_entry*)((uintptr_t)entry +
                                              mmap->entry_size)) {
    uint64_t entry_start = entry->addr;
    uint64_t entry_end = entry->addr + entry->len;
    if (entry_end > kernel_end && entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      kprintf("[kernel] memory region: base %x, length %x \n",
              (uint32_t)entry->addr, (uint32_t)entry->len);
      uintptr_t usable_start =
          ALIGN_UP(kernel_end > entry_start ? kernel_end : entry_start, 0x1000);
      uintptr_t usable_size = entry_end - usable_start;
      kprintf("[kernel] initializing kmalloc at %x \n", (uint32_t)heap_base);
      kmalloc_init((uint32_t)usable_start, (uint32_t)usable_size);
      return;  // found it.
    }
  }
  // note to self:
  // you can also use sbrk with this, assuming kmalloc_init is not used.

  if (heap_base == 0) {
    kprintf("[kernel] no suitable memory region found \n");
    asm volatile("hlt");
  }
}

#ifdef KERNEL_MINIMAL
extern int kmain(uint32_t mb_magic, uint32_t mb_info) {
  serial_init();
  if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    kprintf("[kernel] invalid multiboot magic: expected %x got %x \n",
            MULTIBOOT2_BOOTLOADER_MAGIC, mb_magic);
    asm volatile("hlt");
  }

  kprintf("[kernel] init cpu core \n");
  gdt_install();  // global descriptor table
  idt_install();  // interrupt descriptor table
  isr_install();  // interrupt service routines
  irq_install();  // hardware interrupt request
  kprintf("[kernel] cpu core initialized \n");

  kprintf("[kernel] parsing multiboot info \n");
  multiboot_add_callback(MULTIBOOT_TAG_TYPE_MMAP,
                         (multiboot_callback)__multiboot_kmalloc);
  multiboot_start(mb_info);
  kprintf("[kernel] multiboot info parsed \n");
}
#endif