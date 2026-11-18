#ifndef TASK_H
#define TASK_H

#include "aspace.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t x19, x20, x21, x22, x23, x24, x25, x26, x27, x28;
  uint64_t fp;
  uint64_t lr;
  uint64_t sp;
} CPUContext;

#define TASK_STACK_SIZE 8192

enum TaskState {
  TASK_READY,
  TASK_RUNNING,
  TASK_SLEEPING,
  TASK_BLOCKED,
  TASK_DEAD,
};

typedef struct Task {
  CPUContext ctx;
  struct Task *next;
  void *stack;
  enum TaskState state;
  uint64_t id;
  const char *name;
  uint64_t wake_tick;
  ASpace *aspace;
  uint64_t user_entry;
  uint64_t user_sp;
  const void *wait_chan;
  uint64_t parent_id;
  int exit_code;
  int collected;
  uint64_t user_argc;
  uint64_t user_argv;
} Task;

Task *task_create(void (*fn)(void *), void *arg, const char *name);
void task_exit(void) __attribute__((noreturn));

_Static_assert(sizeof(CPUContext) == 104, "CPUContext size");
_Static_assert(offsetof(CPUContext, sp) == 96, "CPUContext sp offset");

void cpu_switch_to(CPUContext *prev, CPUContext *next);

#endif
