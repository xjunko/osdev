#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// consts
volatile uint16_t *vga_buffer = (uint16_t *)0xB8000;
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

// variables
int vga_x = 0;
int vga_y = 0;
uint8_t vga_color = 0x0F; // black, white

void terminal_init() {
  // set all the values in the buffer with our vga_color
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      vga_buffer[index] = ((uint16_t)vga_color << 8) | '+';
    }
  }
}

void terminal_write(char c) {
  switch (c) {

  case '\n': {
    vga_x = 0;
    vga_y++;
    break;
  }

  default: {
    const size_t index = vga_y * VGA_WIDTH + vga_x;
    vga_buffer[index] = ((uint16_t)vga_color << 8) | c;
    vga_x++;
    break;
  }
  }

  // check just in case
  if (vga_x >= VGA_WIDTH) {
    vga_x = 0;
    vga_y++;
  }

  if (vga_y >= VGA_HEIGHT) {
    vga_y = 0;
    vga_x = 0;
  }
}

void terminal_print(const char *text) {
  for (int i = 0; text[i] != '\0'; i++) {
    terminal_write(text[i]);
  }
}

void terminal_color(uint8_t c) { vga_color = c; }