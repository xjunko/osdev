#include <kernel/types.h>
#include <string.h>

int strncmp(const char *_l, const char *_r, u32 n) {
  const unsigned char *l = (void *)_l, *r = (void *)_r;
  if (!n--) return 0;
  for (; *l && *r && n && *l == *r; l++, r++, n--);
  return *l - *r;
}
