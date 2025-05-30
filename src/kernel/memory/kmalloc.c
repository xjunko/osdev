#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/types.h>
#include <stdio.h>
#include <string.h>

// will be filled by multiboot2
static u32 heap_start = 4 * 1024 * 1024;
static int total_usable_memory = 0x0;  // in bytes

// global memory
static struct mem_manager* global_manager = 0;

#define CHUNK_OVERHEAD (sizeof(struct mem_chunk))

void kmalloc_init(struct multiboot_tag_mmap* mmap) {
  printf("[Kernel] Memory Mapping: ");

  for (struct multiboot_mmap_entry* entry = mmap->entries;
       (u8*)entry < (u8*)mmap + mmap->size;
       entry = (struct multiboot_mmap_entry*)((u8*)entry + mmap->entry_size)) {
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
  _kmalloc_init(heap_start, total_usable_memory);
}

void _kmalloc_init(u32 start, u32 size) {
  global_manager = (struct mem_manager*)start;
  global_manager->size = size;

  struct mem_chunk* initial =
      (struct mem_chunk*)(start + sizeof(struct mem_manager));
  initial->next = 0;
  initial->prev = 0;
  initial->allocated = false;
  initial->size = size - sizeof(struct mem_manager) - CHUNK_OVERHEAD;

  global_manager->first = initial;

  printf("[Kernel] Memory initialized at %x with size %d\n", start, size);
}

void* kmalloc(u32 size) {
  if (!global_manager) return 0;

  struct mem_chunk* current = global_manager->first;
  size = (size + 7) & ~7;

  while (current) {
    if (!current->allocated && current->size >= size) {
      if (current->size >= size + CHUNK_OVERHEAD + 8) {
        struct mem_chunk* new_chunk =
            (struct mem_chunk*)((u8*)current + CHUNK_OVERHEAD + size);
        new_chunk->size = current->size - size - CHUNK_OVERHEAD;
        new_chunk->allocated = false;
        new_chunk->next = current->next;
        new_chunk->prev = current;

        if (current->next) {
          current->next->prev = new_chunk;
        }

        current->next = new_chunk;
        current->size = size;
      }

      printf("[Kernel] Allocated %d bytes at %x\n", size,
             (u32)((u8*)current + CHUNK_OVERHEAD));

      current->allocated = true;
      return (void*)((u8*)current + CHUNK_OVERHEAD);
    }

    current = current->next;
  }

  return 0;
}

void* krealloc(void* ptr, u32 new_size) {
  if (!ptr) return kmalloc(new_size);

  if (new_size == 0) {
    kfree(ptr);
    return 0;
  }

  struct mem_chunk* old_chunk = (struct mem_chunk*)((u8*)ptr - CHUNK_OVERHEAD);
  u32 old_size = old_chunk->size;

  if (new_size <= old_size) {
    return ptr;  // No need to reallocate
  }

  void* new_ptr = kmalloc(new_size);
  if (!new_ptr) return 0;

  memcpy(new_ptr, ptr, old_size);
  kfree(ptr);

  return new_ptr;
}

int kfree(void* ptr) {
  if (!ptr) return -1;

  struct mem_chunk* chunk = (struct mem_chunk*)((u8*)ptr - CHUNK_OVERHEAD);

  if (chunk->allocated != true) {
    printf("[Kernel] Warning: Double free or corruption at %x\n", (u32)chunk);
    return -1;
  }

  chunk->allocated = false;

  if (chunk->next && !chunk->next->allocated) {
    chunk->size += CHUNK_OVERHEAD + chunk->next->size;
    chunk->next = chunk->next->next;
    if (chunk->next) {
      chunk->next->prev = chunk;
    }
  }

  if (chunk->prev && !chunk->prev->allocated) {
    chunk->prev->size += CHUNK_OVERHEAD + chunk->size;
    chunk->prev->next = chunk->next;
    if (chunk->next) {
      chunk->next->prev = chunk->prev;
    }
  }

  printf("[Kernel] Freed memory at %x\n", (u32)ptr);

  return 0;
}