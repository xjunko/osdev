#include <kernel/types.h>
#include <string.h>

u32 strlen(const char *s) {
  const char *a = s;
  for (; *s; s++);
  return s - a;
}