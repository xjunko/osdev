#include <ctype.h>
#include <kernel/types.h>

int tolower(int c) {
  if (isupper(c)) return c | 32;
  return c;
}