#pragma once

#include <kernel/types.h>

void vga_init();
void vga_write_registers(uint8_t* registers);
void vga_set_mode(uint32_t width, uint32_t height, uint32_t color_depth);
uint8_t vga_get_color_index(uint8_t r, uint8_t g, uint8_t b);
uint8_t* vga_get_frame_buffer_part();
void vga_put_color_index(uint32_t x, uint32_t y, uint32_t color_index);
void vga_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void vga_draw();