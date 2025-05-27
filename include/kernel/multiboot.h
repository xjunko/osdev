#pragma once
#include <kernel/types.h>
#include <multiboot2.h>
#include <stdlib.h>

typedef void (*multiboot_callback)(void*);

struct multiboot_handler {
  int type;
  multiboot_callback callback[16];
};

void multiboot_add_callback(int, multiboot_callback);

void multiboot_start(u32);