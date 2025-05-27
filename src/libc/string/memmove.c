#include <kernel/types.h>
#include <libc/string.h>

void* memmove(void* dest, const void* src, u32 len) {
  u8* d = (u8*)dest;
  const u8* s = (const u8*)src;

  if (d < s) {
    while (len--) {
      *d++ = *s++;
    }
  } else if (d > s) {
    d += len;
    s += len;
    while (len--) {
      *(--d) = *(--s);
    }
  }

  return dest;
}