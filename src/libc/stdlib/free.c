#include <kernel/memory.h>
#include <kernel/types.h>
#include <stdio.h>

void free(void* ptr) { kfree(ptr); }