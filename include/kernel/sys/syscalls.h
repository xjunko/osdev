#pragma once
#include <kernel/types.h>

void syscall_init(u8);
u32 syscall_handle(u32);