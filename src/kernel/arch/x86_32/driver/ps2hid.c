#include <kernel/idt.h>
#include <kernel/misc/kprintf.h>
#include <kernel/ports.h>
#include <kernel/ps2hid.h>
#include <kernel/regs.h>
#include <kernel/types.h>

#define PS2_DATA 0x60
#define PS2_CMD 0x64

#define KB_IRQ 0x21
#define MOUSE_IRQ 0x2C

// master
int ps2_device_init(struct interrupt_manager* manager) {
  ps2_kb_init(manager);
  ps2_mouse_init(manager);
  return 0;
}

// irq dev 0x01 (keyboard)
int ps2_kb_init(struct interrupt_manager* manager) {
  new_interrupt_handler(KB_IRQ, manager, &ps2_kb_handle);

  while (inportb(PS2_CMD) & 0x1) {
    inportb(PS2_DATA);
    outportb(PS2_CMD, 0xAE);
    outportb(PS2_CMD, 0x20);
    u8 status = (inportb(PS2_DATA) | 1) & ~0x10;
    outportb(PS2_CMD, 0x60);
    outportb(PS2_DATA, status);
    outportb(PS2_DATA, 0xF4);
  }

  return 0;
}

u32 ps2_kb_handle(u32 esp) {
  u8 key = inportb(PS2_DATA);
  return esp;
}

// irq dev 0x0c (mouse)
#define PS2_MOUSE_CALLBACKS_SIZE 8
static u8 mouse_buffer[3];
static u8 mouse_offset = 0;
static u8 mouse_buttons = 0;
static i8 mouse_x = 0;
static i8 mouse_y = 0;
static ps2_mouse_callback mouse_callbacks[PS2_MOUSE_CALLBACKS_SIZE];

int ps2_mouse_init(struct interrupt_manager* manager) {
  new_interrupt_handler(MOUSE_IRQ, manager, &ps2_mouse_handle);

  outportb(PS2_CMD, 0xA8);  // active
  outportb(PS2_CMD, 0x20);  // get current state

  u8 status = inportb(PS2_DATA) | 2;
  outportb(PS2_CMD, 0x60);  // set state
  outportb(PS2_DATA, status);

  outportb(PS2_CMD, 0xD4);
  outportb(PS2_DATA, 0xF4);
  inportb(PS2_DATA);

  for (int i = 0; i < PS2_MOUSE_CALLBACKS_SIZE; i++) {
    mouse_callbacks[i] = 0;
  }
  for (int i = 0; i < 3; i++) {
    mouse_buffer[i] = 0;
  }

  return 0;
}

void ps2_mouse_register_callback(ps2_mouse_callback callback) {
  for (int i = 0; i < PS2_MOUSE_CALLBACKS_SIZE; i++) {
    if (mouse_callbacks[i] == 0) {
      mouse_callbacks[i] = callback;
      return;
    }
  }

  kprintf("[PS2] Error: No space for mouse callback\n");
}

u32 ps2_mouse_handle(u32 esp) {
  u8 status = inportb(PS2_CMD);

  if (!(status & 0x20)) {
    return esp;
  }

  mouse_buffer[mouse_offset] = inportb(PS2_DATA);
  mouse_offset = (mouse_offset + 1) % 3;

  if (mouse_offset == 0) {
    struct ps2_mouse_state state = {};

    mouse_x += mouse_buffer[1];
    mouse_y -= mouse_buffer[2];

    state.x = mouse_x;
    state.y = mouse_y;

    for (int i = 0; i < PS2_MOUSE_CALLBACKS_SIZE; i++) {
      if (mouse_callbacks[i] != 0) {
        mouse_callbacks[i](state);
      }
    }
  }

  return esp;
}