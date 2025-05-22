#pragma once

#include <kernel/types.h>

void vga_init();
void vga_write_registers(u8* registers);
void vga_set_mode(u32 width, u32 height, u32 color_depth);
u8 vga_get_color_index(u8 r, u8 g, u8 b);
u8* vga_get_frame_buffer_part();
void vga_put_color_index(u32 x, u32 y, u32 color_index);
void vga_put_pixel(u32 x, u32 y, u8 r, u8 g, u8 b);