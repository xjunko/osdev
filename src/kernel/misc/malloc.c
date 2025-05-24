#include <kernel/memory.h>
#include <kernel/types.h>
#include <stdio.h>

static struct mem_manager* global_manager = 0;

#define CHUNK_OVERHEAD (sizeof(struct mem_chunk))

void kmemory_init(u32 start, u32 size) {
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

int kfree(void* ptr) {
  if (!ptr) return -1;

  struct mem_chunk* chunk = (struct mem_chunk*)((u8*)ptr - CHUNK_OVERHEAD);
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

  printf("[Kernel] Freed memory at %x\n", (u32)chunk);

  return 0;
}