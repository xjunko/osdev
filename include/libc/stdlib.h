#pragma once
#include <kernel/types.h>

void* malloc(u32 size);
void* realloc(void*, u32);
void free(void* ptr);
