#pragma once
#include <kernel/multiboot.h>
#include <kernel/types.h>

void framebuffer_init(struct multiboot_tag_framebuffer*);
void framebuffer_set_pixel(u32, u32, u32, u32, u32);
void framebuffer_clear(u32, u32, u32);

void framebuffer_fill_rect(u32, u32, u32, u32, u32, u32, u32);
void framebuffer_flush();