#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hardware/pit.c"
#include "libc/str.c"
#include "tty/term.c"

void kernel_main() {
  // hello world.
  terminal_color(0x2A);
  terminal_init();

  // read pit, just for testing.
  terminal_color(0xF0);
  char buf[32];
  uint32_t pit_read = pit_read_count();
  itoa(pit_read, buf);

  terminal_print("reading pit, ");
  for (int i = 0; i < 32; i++) {
    if (buf[i] == 0) {
      break;
    }

    terminal_write(buf[i]);
  }
  terminal_print(", is what it receives from the inb/out. \n");

  // spam
  for (int i = 0; i < 6; i++) {
    terminal_color(0x0B);
    terminal_print("fuck this shit\n");
  }
}