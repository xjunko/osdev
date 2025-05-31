#pragma once
#include <kernel/idt.h>
#include <kernel/types.h>

// master
int ps2_device_init();

// kb
struct ps2_kb_state {
  int page;
  int code;
  int value;
  int flags;
};

typedef void (*ps2_kb_callback)(struct ps2_kb_state state);
int ps2_kb_init();
void ps2_kb_register_callback(ps2_kb_callback);
uint32_t ps2_kb_handle(uint32_t);

// mouse
struct ps2_mouse_state {
  int32_t x;
  int32_t y;
  bool buttons[3];
};
typedef void (*ps2_mouse_callback)(struct ps2_mouse_state state);

int ps2_mouse_init();
void ps2_mouse_register_callback(ps2_mouse_callback);
uint32_t ps2_mouse_handle(uint32_t);
