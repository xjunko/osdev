#include <ctype.h>
#include <kernel/types.h>

int toupper(int c) {
  if (islower(c)) return c & 0x5f;
  return c;
}