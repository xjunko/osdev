#include <kernel/memory.h>
#include <kernel/types.h>
#include <stdio.h>

void* malloc(u32 size) { return kmalloc(size); }
void* realloc(void* ptr, u32 size) { return krealloc(ptr, size); }
void free(void* ptr) { kfree(ptr); }
