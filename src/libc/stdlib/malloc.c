#include <kernel/memory.h>
#include <kernel/types.h>
#include <stdio.h>

void* malloc(u32 size) { return kmalloc(size); }
void* realloc(void* ptr, u32 size) { return krealloc(ptr, size); }
void free(void* ptr) { kfree(ptr); }

void* memcpy(void* dest, const void* src, u32 len) {
  u8* d = (u8*)dest;
  const u8* s = (const u8*)src;

  while (((u32)d & 3) && len) {
    *d++ = *s++;
    len--;
  }

  u32* d32 = (u32*)d;
  const u32* s32 = (const u32*)s;
  while (len >= 4) {
    *d32++ = *s32++;
    len -= 4;
  }

  d = (u8*)d32;
  s = (const u8*)s32;
  while (len--) {
    *d++ = *s++;
  }

  return dest;
}
