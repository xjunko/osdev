#include <kernel/multiboot.h>
#include <kernel/types.h>
#include <kernel/vesa.h>
#include <stdio.h>

struct vesa_info_t* vesa_info;
struct vesa_mode_info_t* vesa_mode_info;

void vesa_init(multiboot_info_t* mb_info) {
  vesa_info = (struct vesa_info_t*)(mb_info->vbe_control_info);
  vesa_mode_info = (struct vesa_mode_info_t*)(mb_info->vbe_mode_info);
}

void vesa_clear(vesa_color color) {}

u32 vesa_device_color(vesa_color color) { return 0; }
