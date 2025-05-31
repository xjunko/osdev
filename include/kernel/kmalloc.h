#pragma once
#include <kernel/multiboot.h>
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

void kmalloc_init(struct multiboot_tag_mmap*);
void* kmalloc(u32);
void* krealloc(void*, u32);
int kfree(void*);
