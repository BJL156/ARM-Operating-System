#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>

typedef struct {
  uint64_t x[31];
  uint64_t elr;
  uint64_t spsr;
  uint64_t sp_el0;
} TrapFrame;

_Static_assert(sizeof(TrapFrame) == 272, "Trap frame size");

#endif
