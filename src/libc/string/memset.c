#include <kernel/types.h>
#include <libc/string.h>

void* memset(void* dest, int c, u32 len) {
  u8* d = (u8*)dest;
  u32 i;

  for (i = 0; i < len; i++) {
    d[i] = (u8)c;
  }

  return dest;
}