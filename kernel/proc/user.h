#ifndef USER_H
#define USER_H

#include <stdint.h>

void enter_user(uint64_t entry, uint64_t sp, uint64_t argc, uint64_t argv) __attribute__((noreturn));

void user_launch(void *arg);

#endif
