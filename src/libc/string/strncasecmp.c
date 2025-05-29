#include <ctype.h>
#include <kernel/types.h>
#include <strings.h>

int strncasecmp(const char *s1, const char *s2, unsigned int n) {
  unsigned int i = 0;

  while (i < n) {
    char c1 = tolower(s1[i]);
    char c2 = tolower(s2[i]);

    if (c1 != c2 || c1 == '\0' || c2 == '\0') {
      return (unsigned char)c1 - (unsigned char)c2;
    }
    i++;
  }

  return 0;  // strings are equal up to n
}