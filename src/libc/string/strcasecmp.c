#include <ctype.h>
#include <kernel/types.h>
#include <strings.h>

int strcasecmp(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    char c1 = tolower(*s1);
    char c2 = tolower(*s2);

    if (c1 != c2) {
      return (unsigned char)c1 - (unsigned char)c2;
    }

    s1++;
    s2++;
  }

  return (unsigned char)tolower(*s1) - (unsigned char)tolower(*s2);
}