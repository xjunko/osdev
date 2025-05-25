#include <kernel/types.h>
#include <string.h>

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
