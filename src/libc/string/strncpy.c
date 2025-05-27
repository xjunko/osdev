#include <kernel/types.h>
#include <string.h>

char* strncpy(char* dest, const char* src, u32 n) {
  char* original_dest = dest;
  u32 i;

  // Copy up to n characters from src to dest
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }

  // If we copied less than n characters, pad the rest with null bytes
  for (; i < n; i++) {
    dest[i] = '\0';
  }

  return original_dest;  // Return the original destination pointer
}