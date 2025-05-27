#include <kernel/types.h>
#include <string.h>

void* memchr(const void* s, int c, u32 n) {
  const unsigned char* p = (const unsigned char*)s;
  for (u32 i = 0; i < n; i++) {
    if (p[i] == (unsigned char)c) {
      return (void*)(p + i);
    }
  }
  return NULL;
}