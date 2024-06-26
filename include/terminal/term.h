#ifndef __LOVE_TERM_H
#define __LOVE_TERM_H

#include <commons/types.h>

namespace RinOS {
namespace Terminal {
void set_position(u8 x, u8 y);
void get_position(u8 *x, u8 *y);
void set_color(u8 color);
void set_pixel(u16 x, u16 y, u8 color, u8 character);
void printf(const char *str);
void log(const char *prefix, const char *str);
void clear();
}  // namespace Terminal
}  // namespace RinOS

#endif  // __LOVE_TERM_H