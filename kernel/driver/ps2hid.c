#include <kernel/framebuffer.h>
#include <kernel/idt.h>
#include <kernel/input.h>
#include <kernel/ports.h>
#include <kernel/ps2hid.h>
#include <kernel/ps2table.h>
#include <kernel/regs.h>
#include <kernel/types.h>
#include <stdio.h>

#define PS2_DATA 0x60
#define PS2_CMD 0x64

#define KB_IRQ 0x21
#define MOUSE_IRQ 0x2C

// master
int ps2_device_init() {
  ps2_kb_init();
  ps2_mouse_init();
  return 0;
}

// irq dev 0x01 (keyboard)
#define PS2_KB_CALLBACKS_SIZE 16
static ps2_kb_callback kb_callbacks[PS2_KB_CALLBACKS_SIZE];

int ps2_kb_init() {
  idt_set_handler(KB_IRQ, &ps2_kb_handle);

  while (inportb(PS2_CMD) & 0x1) {
    inportb(PS2_DATA);
    outportb(PS2_CMD, 0xAE);
    outportb(PS2_CMD, 0x20);
    uint8_t status = (inportb(PS2_DATA) | 1) & ~0x10;
    outportb(PS2_CMD, 0x60);
    outportb(PS2_DATA, status);
    outportb(PS2_DATA, 0xF4);
  }

  return 0;
}

void ps2_kb_register_callback(ps2_kb_callback callback) {
  for (int i = 0; i < PS2_KB_CALLBACKS_SIZE; i++) {
    if (kb_callbacks[i] == 0) {
      kb_callbacks[i] = callback;
      return;
    }
  }

  printf("[PS2] Error: No space for keyboard callback\n");
}

static int kbd_state = 0;
struct ps2_kb_state _ps2_kb_process_(uint8_t key) {
  struct ps2_kb_state state;

  int press, index, page, code;

  press = !(key & SCAN_RELEASE) ? INPUT_PRESS : INPUT_RELEASE;
  index = key & ~SCAN_RELEASE;

  switch (kbd_state) {
    case 1:
      page = scanmap_escaped[index].page;
      code = scanmap_escaped[index].code;
      break;
    case 2:
      kbd_state = (index == 0x1D) ? 3 : 0;
      return state;  // early return
    case 3:
      if (index == SCAN_NUMLOCK) {
        page = INPUT_PAGE_KEY;
        code = INPUT_KEY_PAUSE;
        break;
      }
      /* FALLTHROUGH */
    default:
      switch (key) {
        case SCAN_EXT0:
          kbd_state = 1;
          return state;
        case SCAN_EXT1:
          kbd_state = 2;
          return state;
      }

      page = scanmap_normal[index].page;
      code = scanmap_normal[index].code;
      break;
  }

  kbd_state = 0;

  state.page = page;
  state.code = code;
  state.value = press;
  state.flags = 0;

  return state;
}

uint32_t ps2_kb_handle(uint32_t esp) {
  uint8_t key = inportb(PS2_DATA);
  struct ps2_kb_state state = _ps2_kb_process_(key);
  for (int i = 0; i < PS2_KB_CALLBACKS_SIZE; i++) {
    if (kb_callbacks[i] != 0) {
      kb_callbacks[i](state);
    }
  }
  return esp;
}

// irq dev 0x0c (mouse)
#define PS2_MOUSE_CALLBACKS_SIZE 8
static uint8_t mouse_buffer[3];
static uint8_t mouse_offset = 0;
static uint8_t mouse_buttons = 0;
static int32_t mouse_x = 0;
static int32_t mouse_y = 0;
static int32_t mouse_max_x = -1;
static int32_t mouse_max_y = -1;
static ps2_mouse_callback mouse_callbacks[PS2_MOUSE_CALLBACKS_SIZE];

int ps2_mouse_init() {
  // initialize irq handler
  idt_set_handler(MOUSE_IRQ, &ps2_mouse_handle);

  // sets the mouse max res based on fb's resolution
  struct framebuffer_info fb_info = framebuffer_get_info();
  mouse_max_x = (int32_t)fb_info.width;
  mouse_max_y = (int32_t)fb_info.height;

  if (mouse_max_x <= 0 || mouse_max_y <= 0) {
    printf(
        "[PS2] Error: Invalid framebuffer resolution for mouse, assuming "
        "1024x768.\n");
    mouse_max_x = 1024;
    mouse_max_y = 768;
    return -1;
  }

  printf("[PS2] Mouse max resolution: %dx%d\n", mouse_max_x, mouse_max_y);

  // irq acks
  outportb(PS2_CMD, 0xA8);  // active
  outportb(PS2_CMD, 0x20);  // get current state

  uint8_t status = inportb(PS2_DATA) | 2;
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

  printf("[PS2] Error: No space for mouse callback\n");
}

uint32_t ps2_mouse_handle(uint32_t esp) {
  uint8_t status = inportb(PS2_CMD);

  if (!(status & 0x20)) {
    return esp;
  }

  mouse_buffer[mouse_offset] = inportb(PS2_DATA);
  mouse_offset = (mouse_offset + 1) % 3;

  if (mouse_offset == 0) {
    struct ps2_mouse_state state;

    mouse_x += (int8_t)mouse_buffer[2];
    mouse_y -= (int8_t)mouse_buffer[0];

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= mouse_max_x) mouse_x = mouse_max_x - 1;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y > mouse_max_y) mouse_y = mouse_max_y - 1;

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