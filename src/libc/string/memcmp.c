#include <kernel/types.h>
#include <string.h>

int memcmp(const void* vl, const void* vr, u32 n) {
  const unsigned char *l = vl, *r = vr;
  for (; n && *l == *r; n--, l++, r++);
  return n ? *l - *r : 0;
}