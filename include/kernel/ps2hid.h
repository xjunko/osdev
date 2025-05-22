#include <kernel/idt.h>
#include <kernel/types.h>

int ps2_kb_init(struct interrupt_manager*);
u32 ps2_kb_handle(u32);
