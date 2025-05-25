#include <kernel/multiboot.h>
#include <kernel/types.h>
#include <stdio.h>

static struct multiboot_handler mb_handlers[32];

void multiboot_add_callback(int type, multiboot_callback callback) {
  // safe to assume all of them are <= 21
  for (int i = 0; i < 16; i++) {
    if (mb_handlers[type].callback[i] == 0) {
      mb_handlers[type].callback[i] = callback;
      return;
    }
  }
}

void multiboot_start(u32 mb_info) {
  struct multiboot_tag *tag;

  u32 size = *(u32 *)mb_info;
  u32 end = mb_info + size;

  for (tag = (struct multiboot_tag *)(mb_info + 8); (u32)tag < end;
       tag = (struct multiboot_tag *)(((u32)tag + tag->size + 7) & ~7)) {
    struct multiboot_handler handler = mb_handlers[tag->type];
    for (int i = 0; i < 16; i++) {
      if (handler.callback[i] != 0) {
        handler.callback[i](tag);
      }
    }
  }
}