// poor man's libc for string
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

int itoa(int value, char *ptr) {
  int count = 0, temp;
  if (ptr == NULL)
    return 0;
  if (value == 0) {
    *ptr = '0';
    return 1;
  }

  if (value < 0) {
    value *= (-1);
    *ptr++ = '-';
    count++;
  }

  for (temp = value; temp > 0; temp /= 10, ptr++)
    ;
  *ptr = '\0';

  for (temp = value; temp > 0; temp /= 10) {
    *--ptr = temp % 10 + '0';
    count++;
  }
  return count;
}