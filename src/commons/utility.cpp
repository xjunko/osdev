#include <commons/utility.h>

void Utility::wait_for_a_bit(int how_long) {
  for (int i = 0; i < how_long; i++) {
    for (int j = 0; j < 1000000; j++) {
      __asm__("nop");
    }
  }
}