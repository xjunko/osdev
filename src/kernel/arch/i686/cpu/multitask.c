#include <kernel/gdt.h>
#include <kernel/multitask.h>
#include <kernel/regs.h>
#include <kernel/types.h>
#include <stdio.h>

static struct cpu_manager main_cpu = {.num_tasks = 0, .cur_tasks = 0};

struct cpu_task cpu_new_task(cpu_task_entrypoint entry) {
  struct cpu_task new_task = {};
  new_task.registers =
      (struct regs*)(new_task.stack + 4096 - sizeof(struct regs));

  new_task.registers->eax = 0;
  new_task.registers->ebx = 0;
  new_task.registers->ecx = 0;
  new_task.registers->edx = 0;

  new_task.registers->esi = 0;
  new_task.registers->edi = 0;
  new_task.registers->ebp = 0;

  new_task.registers->error = 0;

  new_task.registers->esp =
      (u32)(new_task.stack + TASK_STACK - sizeof(struct regs));
  ;
  new_task.registers->eip = (u32)entry;
  new_task.registers->cs = code_segment_selector();
  new_task.registers->ss = 0;
  new_task.registers->eflags = 0x202;

  return new_task;
}

bool cpu_add_task(struct cpu_task* task) {
  if (main_cpu.num_tasks >= 256) return false;
  main_cpu.tasks[main_cpu.num_tasks++] = task;
  kprintf("[CPU] Added task: ID=%d \n", main_cpu.num_tasks - 1);
  return true;
}

struct regs* cpu_schedule(struct regs* registers) {
  if (main_cpu.num_tasks <= 0) return registers;

  if (main_cpu.cur_tasks >= 0) {
    main_cpu.tasks[main_cpu.cur_tasks]->registers = registers;
  }

  if (++main_cpu.cur_tasks >= main_cpu.num_tasks)
    main_cpu.cur_tasks %= main_cpu.num_tasks;

  return main_cpu.tasks[main_cpu.cur_tasks]->registers;
}