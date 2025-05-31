#pragma once
#include <kernel/multiboot.h>
#include <kernel/types.h>

struct mem_chunk {
  struct mem_chunk* next;
  struct mem_chunk* prev;
  bool allocated;
  uint32_t size;
};

struct mem_manager {
  struct mem_chunk* first;
  uint32_t size;
};

void kmalloc_init(struct multiboot_tag_mmap*);
void* kmalloc(uint32_t);
void* krealloc(void*, uint32_t);
int kfree(void*);
