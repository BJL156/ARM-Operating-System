#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

static inline uint64_t irq_save(void) {
  uint64_t flags;
  asm volatile("mrs %0, daif\n\tmsr daifset, #2" : "=r"(flags) :: "memory");
  return flags;
}

static inline void irq_restore(uint64_t flags) {
  asm volatile("msr daif, %0" :: "r"(flags) : "memory");
}

#endif
