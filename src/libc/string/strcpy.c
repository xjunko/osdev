#include <kernel/types.h>
#include <string.h>

char* strcpy(char* dest, const char* src) {
  char* original_dest = dest;
  while (*src != '\0') {
    *dest++ = *src++;
  }
  *dest = '\0';          // Null-terminate the destination string
  return original_dest;  // Return the original destination pointer
}