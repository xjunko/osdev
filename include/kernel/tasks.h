#pragma once
#include <kernel/types.h>

#define TASK_STACK 4096

typedef void (*cpu_task_entrypoint)();

enum cpu_state { ready, running, halted };

struct cpu_task {
  u32 id;
  u8 stack[TASK_STACK];
  struct regs* registers;
  enum cpu_state state;
};

struct cpu_manager {
  struct cpu_task* tasks[256];
  int num_tasks;
  int cur_tasks;
};

struct cpu_task* cpu_task_new(cpu_task_entrypoint);
void cpu_task_free(struct cpu_task*);
bool cpu_task_add(struct cpu_task*);
void cpu_task_remove(u32);

u32 cpu_schedule(u32);
