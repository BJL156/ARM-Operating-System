#include "sched.h"

#include "aspace.h"
#include "irq.h"
#include "kheap.h"
#include "panic.h"
#include "timer.h"

static Task boot_task;
static Task *current;
static Task *tail;

void sched_init(void) {
  boot_task.state = TASK_RUNNING;
  boot_task.name = "boot";
  boot_task.id = 0;
  boot_task.next = &boot_task;
  boot_task.aspace = aspace_kernel();
  
  current = &boot_task;
  tail = &boot_task;
}

static void sched_add_unlocked(Task *task) {
  if (task == NULL) {
    panic("Sched: NULL task.");
  }

  task->next = tail->next;
  tail->next = task;
  tail = task;
}

void sched_add(Task *task) {
  uint64_t flags = irq_save();
  sched_add_unlocked(task);
  irq_restore(flags);
}

void yield(void) {
  uint64_t flags = irq_save();

  Task *prev = current;
  Task *next = prev->next;

  while (next != prev && next->state != TASK_READY) {
    next = next->next;
  }

  if (next == prev) {
    if (prev->state == TASK_DEAD) {
      panic("Sched: no runnable tasks.");
    }

    irq_restore(flags);
    return;
  }

  if (prev->state == TASK_RUNNING) {
    prev->state = TASK_READY;
  }
  next->state = TASK_RUNNING;
  current = next;

  if (next->aspace != prev->aspace) {
    aspace_switch(next->aspace);
  }

  cpu_switch_to(&prev->ctx, &next->ctx);

  irq_restore(flags);
}

Task *sched_current(void) {
  return current;
}

static uint64_t ticks;

void sched_tick(void) {
  ticks++;

  Task *task = &boot_task;
  do {
    if (task->state == TASK_SLEEPING && ticks >= task->wake_tick) {
      task->state = TASK_READY;
    }
    task = task->next;
  } while (task != &boot_task);
}

void task_sleep_ms(uint64_t ms) {
  uint64_t flags = irq_save();

  uint64_t delta = (ms * TICK_HZ + 999) / 1000;
  if (delta == 0) {
    delta = 1;
  }

  current->wake_tick = ticks + delta;
  current->state = TASK_SLEEPING;

  yield();

  irq_restore(flags);
}

void sched_reap(void) {
  uint64_t flags = irq_save();

  Task *prev = &boot_task;
  Task *task = boot_task.next;

  while (task != &boot_task) {
    Task *next = task->next;

    if (task->state == TASK_DEAD && task != current) {
      prev->next = next;
      if (tail == task) {
        tail = prev;
      }

      if (task->aspace != aspace_kernel()) {
        aspace_destroy(task->aspace);
      }

      kfree(task->stack);
      kfree(task);
    } else {
      prev = task;
    }

    task = next;
  }

  irq_restore(flags);
}

void sched_block(const void *chan) {
  current->wait_chan = chan;
  current->state = TASK_BLOCKED;
  yield();
}

void sched_wake(const void *chan) {
  uint64_t flags = irq_save();

  Task *task = &boot_task;

  do {
    if (task->state == TASK_BLOCKED && task->wait_chan == chan) {
      task->state = TASK_READY;
      task->wait_chan = NULL;
    }
    task = task->next;
  } while (task != &boot_task);

  irq_restore(flags);
}

Task *sched_find(uint64_t id) {
  Task *task = &boot_task;

  do {
    if (task->id == id) {
      return task;
    }

    task = task->next;
  } while (task != &boot_task);

  return NULL;
}
