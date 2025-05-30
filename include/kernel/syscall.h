#pragma once
#include <kernel/regs.h>
#include <kernel/types.h>

typedef u32(syscall_callback)(struct regs*);

void syscall_init();
void syscall_register(u32, syscall_callback*);
u32 syscall_handle(u32);
