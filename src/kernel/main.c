#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hardware/pit.c"
#include "libc/str.c"

#include "tty/madoka.c"
#include "tty/term.c"

void kernel_main() {
  // hello world.
  terminal_color(0x0F);
  terminal_init();

  // read pit, just for testing.
  terminal_color(0xF0);
  char buf[32];
  uint32_t pit_read = pit_read_count();
  itoa(pit_read, buf);

  draw_madoka();

  terminal_print("MadokaOS v0.0.1 | interrupt=");

  for (int i = 0; i < 32; i++) {
    if (buf[i] == 0) {
      break;
    }

    terminal_write(buf[i]);
  }

  // Skip below logo
  vga_x = 0;
  vga_y = 17;
  terminal_color(0x0C);
  terminal_print("we all love madoka \n");
  terminal_print("we all love madoka \n");
  terminal_print("we all love madoka \n");
}