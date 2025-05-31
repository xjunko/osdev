#include <kernel/gdt.h>
#include <kernel/memory.h>
#include <kernel/multitask.h>
#include <kernel/regs.h>
#include <kernel/types.h>
#include <stdio.h>

static struct cpu_manager main_cpu = {.num_tasks = 0, .cur_tasks = -1};

struct cpu_task* cpu_new_task(cpu_task_entrypoint entry) {
  struct cpu_task* new_task = kmalloc(sizeof(struct cpu_task));
  if (new_task == NULL) {
    kprintf("[CPU] Failed to allocate memory for new task\n");
    return NULL;
  }

  struct regs* regs =
      (struct regs*)(new_task->stack + TASK_STACK - sizeof(struct regs));

  regs->eax = 0;
  regs->ebx = 0;
  regs->ecx = 0;
  regs->edx = 0;
  regs->esi = 0;
  regs->edi = 0;
  regs->ebp = 0;
  regs->error = 0;

  regs->esp = 0;
  regs->eip = (u32)entry;
  regs->cs = code_segment_selector();
  regs->ss = data_segment_selector();
  regs->eflags = 0x202;
  new_task->registers = regs;
  return new_task;
}

bool cpu_add_task(struct cpu_task* task) {
  if (main_cpu.num_tasks >= 256) return false;
  main_cpu.tasks[main_cpu.num_tasks++] = task;
  kprintf("[CPU] Added task: ID=%d \n", main_cpu.num_tasks - 1);
  return true;
}

u32 cpu_schedule(u32 esp) {
  if (main_cpu.num_tasks <= 0) return esp;

  if (main_cpu.cur_tasks >= 0) {
    main_cpu.tasks[main_cpu.cur_tasks]->registers = (struct regs*)esp;
  }

  if (++main_cpu.cur_tasks >= main_cpu.num_tasks)
    main_cpu.cur_tasks %= main_cpu.num_tasks;

  return (u32)main_cpu.tasks[main_cpu.cur_tasks]->registers;
}