#include <kernel/idt.h>
#include <kernel/types.h>

// master
int ps2_device_init();

// kb
int ps2_kb_init();
u32 ps2_kb_handle(u32);

// mouse
struct ps2_mouse_state {
  i32 x;
  i32 y;
  bool buttons[3];
};
typedef void (*ps2_mouse_callback)(struct ps2_mouse_state state);

int ps2_mouse_init();
void ps2_mouse_register_callback(ps2_mouse_callback);
u32 ps2_mouse_handle(u32);
