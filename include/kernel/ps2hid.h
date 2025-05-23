#include <kernel/idt.h>
#include <kernel/types.h>

// master
int ps2_device_init(struct interrupt_manager*);

// kb
int ps2_kb_init(struct interrupt_manager*);
u32 ps2_kb_handle(u32);

// mouse
struct ps2_mouse_state {
  i8 x;
  i8 y;
  bool buttons[3];
};
typedef void (*ps2_mouse_callback)(struct ps2_mouse_state state);

int ps2_mouse_init(struct interrupt_manager*);
void ps2_mouse_register_callback(ps2_mouse_callback);
u32 ps2_mouse_handle(u32);
