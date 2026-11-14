#include "sched.h"

#include "panic.h"

static Task boot_task;
static Task *current;
static Task *tail;

void sched_init(void) {
  boot_task.state = TASK_RUNNING;
  boot_task.name = "boot";
  boot_task.id = 0;
  boot_task.next = &boot_task;
  
  current = &boot_task;
  tail = &boot_task;
}

void sched_add(Task *task) {
  if (task == NULL) {
    panic("Sched: NULL task.");
  }

  task->next = tail->next;
  tail->next = task;
  tail = task;
}

void yield(void) {
  Task *prev = current;
  Task *next = prev->next;

  while (next != prev && next->state != TASK_READY) {
    next = next->next;
  }

  if (next == prev) {
    if (prev->state == TASK_DEAD) {
      panic("Sched: no runnable tasks.");
    }

    return;
  }

  if (prev->state == TASK_RUNNING) {
    prev->state = TASK_READY;
  }

  next->state = TASK_RUNNING;
  current = next;

  cpu_switch_to(&prev->ctx, &next->ctx);
}

Task *sched_current(void) {
  return current;
}
