#include <kernel/types.h>

struct mem_chunk {
  struct mem_chunk* next;
  struct mem_chunk* prev;
  bool allocated;
  u32 size;
};

struct mem_manager {
  struct mem_chunk* first;
  u32 size;
};

struct mem_manager* new_memory_manager(u32, u32);

void* malloc(u32);
int free(void*);

void _init_memory(u32, u32);