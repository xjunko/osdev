#include <kernel/kmalloc.h>
#include <kernel/kprintf.h>
#include <kernel/multiboot.h>
#include <kernel/types.h>
#include <stdio.h>
#include <string.h>

// will be filled by multiboot2
static uint32_t heap_start = 4 * 1024 * 1024;
static int total_usable_memory = 0x0;  // in bytes

// global memory
static struct mem_manager* global_manager = 0;

#define CHUNK_OVERHEAD (sizeof(struct mem_chunk))

void kmalloc_init(uint32_t start, uint32_t size) {
  global_manager = (struct mem_manager*)start;
  global_manager->size = size;

  struct mem_chunk* initial =
      (struct mem_chunk*)(start + sizeof(struct mem_manager));
  initial->next = 0;
  initial->prev = 0;
  initial->allocated = false;
  initial->size = size - sizeof(struct mem_manager) - CHUNK_OVERHEAD;

  global_manager->first = initial;

  kprintf("[Kernel] Memory initialized at %x with size %dMiB\n", start,
          size / 1024 / 1024);
}

void* kmalloc(uint32_t size) {
  if (!global_manager) return 0;

  struct mem_chunk* current = global_manager->first;
  size = (size + 7) & ~7;

  while (current) {
    if (!current->allocated && current->size >= size) {
      if (current->size >= size + CHUNK_OVERHEAD + 8) {
        struct mem_chunk* new_chunk =
            (struct mem_chunk*)((uint8_t*)current + CHUNK_OVERHEAD + size);
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

      kprintf("[Kernel] Allocated %d bytes at %x\n", size,
              (uint32_t)((uint8_t*)current + CHUNK_OVERHEAD));

      current->allocated = true;
      return (void*)((uint8_t*)current + CHUNK_OVERHEAD);
    }

    current = current->next;
  }

  return 0;
}

void* krealloc(void* ptr, uint32_t new_size) {
  if (!ptr) return kmalloc(new_size);

  if (new_size == 0) {
    kfree(ptr);
    return 0;
  }

  struct mem_chunk* old_chunk =
      (struct mem_chunk*)((uint8_t*)ptr - CHUNK_OVERHEAD);
  uint32_t old_size = old_chunk->size;

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

  struct mem_chunk* chunk = (struct mem_chunk*)((uint8_t*)ptr - CHUNK_OVERHEAD);

  if (chunk->allocated != true) {
    kprintf("[Kernel] Warning: Double free or corruption at %x\n",
            (uint32_t)chunk);
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

  kprintf("[Kernel] Freed memory at %x\n", (uint32_t)ptr);

  return 0;
}