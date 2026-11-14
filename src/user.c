#include "sched.h"
#include "task.h"
#include "user.h"

#define USER_STACK_SIZE 16384


void user_launch(void *arg) {
  (void)arg;
  Task *task = sched_current();
  enter_user(task->user_entry, task->user_sp);
}
