#include "task.h"

#include "kheap.h"
#include "kprintf.h"
#include "mem.h"
#include "panic.h"
#include "sched.h"

extern void task_entry_trampoline(void);

static uint64_t next_task_id = 1;

Task *task_create(void (*fn)(void *), void *arg, const char *name) {
  Task *task =  kmalloc(sizeof(Task));
  void *stack = kmalloc(TASK_STACK_SIZE);

  if (task == NULL || stack == NULL) {
    kfree(task);
    kfree(stack);
    
    return NULL;
  }

  memset(task, 0, sizeof(Task));

  task->stack = stack;
  task->state = TASK_READY;
  task->id    = next_task_id++;
  task->name  = name;

  task->ctx.x19 = (uint64_t)fn;
  task->ctx.x20 = (uint64_t)arg;
  task->ctx.lr  = (uint64_t)task_entry_trampoline;
  task->ctx.sp  = (uint64_t)stack + TASK_STACK_SIZE;

  return task;
}

void task_exit(void) {
  Task *task = sched_current();
  
  kprintf("Task %s exited.\n", task->name);
  task->state = TASK_DEAD;
  yield();

  panic("Task: dead task resumed.");
}
