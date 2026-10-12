#include "kprintf.h"
#include "trap.h"
#include "panic.h"

#include <stdint.h>

#define READ_SYSREG(name) ({ \
    uint64_t v; \
    asm volatile("mrs %0, " #name : "=r"(v)); \
    v; \
})

static const char *esr_class_name(uint64_t ec) {
  switch (ec) {
    case 0x00: {
      return "Unknown reason";
    }
    case 0x15: {
      return "SVC (AArch64)";
    }
    case 0x20: {
      return "Instruction abort, lower EL";
    }
    case 0x21: {
      return "Instruction abort, same EL";
    }
    case 0x22: {
      return "PC alignment fault";
    }
    case 0x24: {
      return "Data abort, lower EL";
    }
    case 0x25: {
      return "Data abort, same EL";
    }
    case 0x26: {
      return "SP alignment fault";
    }
    default: {
      return "Other";
    }
  }
}

void exception_handler(TrapFrame *tf, uint64_t idx) {
  kprintf("EXCEPTION: slot %lu\n", idx);

  uint64_t esr = READ_SYSREG(esr_el1);
  uint64_t ec = (esr >> 26) & 0x3F;
  kprintf("ESR_EL1: 0x%lX\n", esr);
  kprintf("DFSC: 0x%lX\n", esr & 0x3F);
  kprintf("CLASS: 0x%lx (%s)\n", ec, esr_class_name(ec));
  kprintf("ELR_EL1: 0x%lX\n", READ_SYSREG(elr_el1));
  kprintf("FAR_EL1: 0x%lX\n", READ_SYSREG(far_el1));
  kprintf("frame ELR: 0x%lX\n", tf->elr);
  kprintf("frame SPSR: 0x%lX\n", tf->spsr);
  
  panic("Unhandled exception");
}
