#pragma once
#include <kernel/regs.h>
#include <kernel/types.h>

typedef uint32_t(syscall_callback)(struct regs*);

void syscall_init();
void syscall_register(uint32_t, syscall_callback*);
uint32_t syscall_handle(uint32_t);
