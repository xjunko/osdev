#include <kernel/gdt.h>
#include <kernel/kmalloc.h>
#include <kernel/kprintf.h>
#include <kernel/regs.h>
#include <kernel/tasks.h>
#include <kernel/types.h>
#include <stdio.h>
#include <string.h>

static struct cpu_manager main_cpu = {.num_tasks = 0, .cur_tasks = -1};

struct cpu_task* cpu_task_new(cpu_task_entrypoint entry) {
  struct cpu_task* new_task = kmalloc(sizeof(struct cpu_task));
  if (new_task == NULL) {
    kprintf("[CPU] Failed to allocate memory for new task\n");
    return NULL;
  }

  struct regs* regs =
      (struct regs*)(new_task->stack + TASK_STACK - sizeof(struct regs));
  memset(regs, 0, sizeof(struct regs));

  regs->eip = (uint32_t)entry;
  regs->cs = code_segment_selector();
  regs->ss = data_segment_selector();
  regs->eflags = 0x202;

  new_task->registers = regs;
  return new_task;
}

void cpu_task_free(struct cpu_task* task) {
  if (task == NULL) return;
  kprintf("[CPU] Freeing task: ID=%d \n", task->id);
  kfree(task->stack);
  kfree(task);
}

bool cpu_task_add(struct cpu_task* task) {
  if (main_cpu.num_tasks >= 256) return false;
  main_cpu.tasks[main_cpu.num_tasks++] = task;
  kprintf("[CPU] Added task: ID=%d \n", main_cpu.num_tasks - 1);
  return true;
}

void cpu_task_remove(int id) {
  if (id >= main_cpu.num_tasks) return;

  kprintf("[CPU] Removing task: ID=%d \n", id);
  cpu_task_free(main_cpu.tasks[id]);

  for (int i = id; i < main_cpu.num_tasks - 1; i++) {
    main_cpu.tasks[i] = main_cpu.tasks[i + 1];
  }
  main_cpu.num_tasks--;
  if (main_cpu.cur_tasks >= main_cpu.num_tasks) {
    main_cpu.cur_tasks = -1;
  }
}

uint32_t cpu_schedule(uint32_t esp) {
  if (main_cpu.num_tasks <= 0) return esp;

  if (main_cpu.cur_tasks >= 0) {
    main_cpu.tasks[main_cpu.cur_tasks]->registers = (struct regs*)esp;
  }

  if (++main_cpu.cur_tasks >= main_cpu.num_tasks)
    main_cpu.cur_tasks %= main_cpu.num_tasks;

  return (uint32_t)main_cpu.tasks[main_cpu.cur_tasks]->registers;
}