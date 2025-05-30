#pragma once
#include <kernel/types.h>

#define TASK_STACK 4096

typedef void (*cpu_task_entrypoint)();

struct cpu_task {
  u8 stack[TASK_STACK];
  struct regs* registers;
};

struct cpu_manager {
  struct cpu_task* tasks[256];
  int num_tasks;
  int cur_tasks;
};

struct cpu_task cpu_new_task(cpu_task_entrypoint);
bool cpu_add_task(struct cpu_task*);
struct regs* cpu_schedule(struct regs*);