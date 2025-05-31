#include <flanterm/backends/fb.h>
#include <flanterm/flanterm.h>
#include <kernel/framebuffer.h>
#include <kernel/tty.h>
#include <stdio.h>

#define _tty_queue_size 4096
static u8 _tty_queue[_tty_queue_size];
static int _tty_queue_i = 0;

static struct framebuffer_info fb_info;
static struct flanterm_context *fb_ctx;

void tty_init() {
  printf("[tty] initializing tty0 \n");
  fb_info = framebuffer_get_info();

  if (fb_info.addr == NULL) {
    printf("[tty] framebuffer not found, cannot initialize tty0\n");
    return;
  }
  printf("[tty] framebuffer found at %p, %ux%u, %u bpp\n", fb_info.addr,
         fb_info.width, fb_info.height, fb_info.bpp);

  printf("[tty] initializing flanterm context \n");
  fb_ctx = flanterm_fb_init(
      malloc, free, fb_info.addr, fb_info.width, fb_info.height, fb_info.pitch,
      fb_info.red_mask_size, fb_info.red_field_position,
      fb_info.green_mask_size, fb_info.green_field_position,
      fb_info.blue_mask_size, fb_info.blue_field_position, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0);
}

bool tty_ready() { return fb_ctx != NULL; }

void tty_flush() {
  if (!tty_ready()) {
    printf("[tty] fb_ctx is NULL, cannot flush tty\n");
    return;
  }
  flanterm_flush(fb_ctx);
}

void tty_write(const char *str) {
  if (!tty_ready()) {
    // write to queue for now.
    if (_tty_queue_i >= _tty_queue_size) {
      _tty_queue[_tty_queue_size - 1] = '\n';
      return;  // silently fails.
    }
    _tty_queue[_tty_queue_i++] = *str;
    return;
  }

  // clears the queue first
  if (_tty_queue_i != 0) {
    for (int i = 0; i < _tty_queue_i; i++) {
      flanterm_write(fb_ctx, (const char *)&_tty_queue[i], 1);
      _tty_queue[i] = 0;
    }
    _tty_queue_i = 0;
  }

  // current write
  flanterm_write(fb_ctx, str, strlen(str));
}