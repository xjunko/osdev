#include <kernel/misc/kprintf.h>
#include <kernel/types.h>
#include <stdlib/malloc.h>

static struct mem_manager* active_manager;

struct mem_manager* new_memory_manager(u32 start, u32 size) {
  if (size < sizeof(struct mem_manager) + sizeof(struct mem_chunk)) {
    return 0;
  }

  struct mem_manager* manager = (struct mem_manager*)start;
  struct mem_chunk* chunk =
      (struct mem_chunk*)(start + sizeof(struct mem_manager));

  chunk->allocated = false;
  chunk->prev = 0;
  chunk->next = 0;
  chunk->size = size - sizeof(struct mem_manager) - sizeof(struct mem_chunk);
  manager->first = chunk;

  kprintf("[Memory] Initialized! \n");

  return manager;
}

void* malloc(u32 size) {
  active_manager->size += size;

  struct mem_chunk* result = 0;

  for (struct mem_chunk* chunk = active_manager->first;
       chunk != 0 && result == 0; chunk = chunk->next) {
    if (chunk->size > size && !chunk->allocated) {
      result = chunk;
    }
  }

  if (!result) {
    return 0;
  }

  if (result->size >= size + sizeof(struct mem_chunk) + 1) {
    struct mem_chunk* temp =
        (struct mem_chunk*)((u32)result + sizeof(struct mem_chunk) + size);

    temp->allocated = false;
    temp->size = result->size - size - sizeof(struct mem_chunk);
    temp->prev = result;
    temp->next = result->next;

    if (temp->next != 0) {
      temp->next->prev = temp;
    }

    result->size = size;
    result->next = temp;
  }
  result->allocated = true;

  return (void*)(((u32)result) + sizeof(struct mem_chunk));
}

int free(void* ptr) {
  struct mem_chunk* chunk =
      (struct mem_chunk*)((u32)ptr - sizeof(struct mem_chunk));
  chunk->allocated = false;

  if (chunk->prev != 0 && !chunk->prev->allocated) {
    chunk->prev->next = chunk->next;
    chunk->prev->size += chunk->size + sizeof(struct mem_chunk);

    if (chunk->next != 0) {
      chunk->next->prev = chunk->prev;
    }

    chunk = chunk->prev;
  }

  if (chunk->next != 0 && !chunk->next->allocated) {
    chunk->size += chunk->next->size + sizeof(struct mem_chunk);
    chunk->next = chunk->next->next;

    if (chunk->next != 0) {
      chunk->next->prev = chunk;
    }
  }

  return 0;
}

void _init_memory(u32 start, u32 size) {
  void* mem_man = new_memory_manager(start, size);
  active_manager = mem_man;
}