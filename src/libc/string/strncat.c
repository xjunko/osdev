#include <kernel/types.h>
#include <string.h>

char *strncat(char *d, const char *s, u32 n) {
  char *a = d;
  d += strlen(d);
  while (n && *s) n--, *d++ = *s++;
  *d++ = 0;
  return a;
}
