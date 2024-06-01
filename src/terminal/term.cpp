#include <commons/types.h>
#include <terminal/term.h>

static u16 *C_VIDEO_MEMORY = (u16 *)0xb8000;

const u16 C_VIDEO_WIDTH = 80;
const u16 C_VIDEO_HEIGHT = 25;

static u8 TERM_X = 0;
static u8 TERM_Y = 0;
static u8 VGA_COLOR = 0x0F;

void RinOS::Terminal::set_position(u8 x, u8 y) {
  TERM_X = x;
  TERM_Y = y;
}

void RinOS::Terminal::set_color(u8 color) { VGA_COLOR = color; }

void RinOS::Terminal::set_pixel(u16 x, u16 y, u8 color, u8 character) {
  C_VIDEO_MEMORY[y * 80 + x] = ((u16)color << 8) | character;
}

void RinOS::Terminal::printf(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    switch (str[i]) {
      case '\n':
        TERM_Y++;
        TERM_X = 0;
        break;

      default:
        C_VIDEO_MEMORY[TERM_Y * 80 + TERM_X] = ((u16)VGA_COLOR << 8) | str[i];
        TERM_X++;
        break;
    }

    if (TERM_X >= 80) {
      TERM_Y++;
      TERM_X = 0;
    }

    if (TERM_Y > 25) {
      RinOS::Terminal::clear();
    }
  }
}

void RinOS::Terminal::log(const char *prefix, const char *str) {
  RinOS::Terminal::set_color(0x0F);
  RinOS::Terminal::printf("[");
  RinOS::Terminal::set_color(0x0A);
  RinOS::Terminal::printf(prefix);
  RinOS::Terminal::set_color(0x0F);
  RinOS::Terminal::printf("] ");
  RinOS::Terminal::printf(str);
  RinOS::Terminal::printf("\n");
  RinOS::Terminal::set_color(0x0F);
}

void RinOS::Terminal::clear() {
  for (TERM_Y = 0; TERM_Y < 25; TERM_Y++) {
    for (TERM_X = 0; TERM_X < 80; TERM_X++) {
      C_VIDEO_MEMORY[TERM_Y * 80 + TERM_X] = ((u16)VGA_COLOR << 8) | ' ';
    }
  }

  TERM_X = 0;
  TERM_Y = 0;
}
