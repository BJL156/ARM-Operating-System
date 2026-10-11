#include "kprintf.h"

#include <stdint.h>

#define READ_SYSREG(name) ({ \
    uint64_t v; \
    asm volatile("mrs %0, " #name : "=r"(v)); \
    v; \
})

void exception_handler(uint64_t idx) {
  kprintf("EXCEPTION: slot %lu\n", idx);

  uint64_t esr = READ_SYSREG(esr_el1);
  kprintf("ESR_EL1: 0x%lX\n", esr);
  kprintf("DFSC: 0x%lX\n", esr & 0x3F);
  kprintf("CLASS: 0x%lX\n", (esr >> 26) & 0x3F);
  kprintf("ELR_EL1: 0x%lX\n", READ_SYSREG(elr_el1));
  kprintf("FAR_EL1: 0x%lX\n", READ_SYSREG(far_el1));

  while (1) {
  }
}
