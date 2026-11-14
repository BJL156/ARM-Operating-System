#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#include "syscall_abi.h"
#include "trap.h"

void syscall_dispatch(TrapFrame *trap_frame);
void user_fault(TrapFrame *trap_frame, uint64_t esr) __attribute__((noreturn));

#endif
