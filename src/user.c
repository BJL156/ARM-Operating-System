#include "memmap.h"
#include "panic.h"
#include "task.h"
#include "user.h"

#define USER_STACK_SIZE 16384

static uint64_t user_brk = USER_START;

uint64_t user_stack_alloc(void) {
  if (user_brk + USER_STACK_SIZE > USER_START + USER_SIZE) {
    panic("User region exhausted");
  }
  user_brk += USER_STACK_SIZE;

  return user_brk;
}

void user_launch(void *arg) {
  uint64_t entry = (uint64_t)arg;
  enter_user(entry, user_stack_alloc());
  panic("enter_user returned");
}
