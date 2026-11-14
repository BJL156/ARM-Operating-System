#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#include "trap.h"

#define SYS_WRITE 1
#define SYS_SLEEP 2
#define SYS_EXIT  3
#define SYS_GETPID 4

void syscall_dispatch(TrapFrame *trap_frame);
void user_fault(TrapFrame *trap_frame, uint64_t esr) __attribute__((noreturn));

#endif
