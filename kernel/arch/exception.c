#include "gic.h"
#include "kprintf.h"
#include "sysreg.h"
#include "timer.h"
#include "trap.h"
#include "panic.h"
#include "sched.h"
#include "syscall.h"
#include "uart.h"

#include <stdint.h>

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
  if (idx == 5 || idx == 9) {
    uint32_t id = gic_ack();

    if (id == 1023) {
      return;
    }

    if (id == 30) {
      timer_start(timer_freq() / TICK_HZ);
      sched_tick();
    }

    if (id == UART_IRQ) {
      uart_rx_irq();
    } else if (id != 30) {
      kprintf("unexpected IRQ %lu\n", (uint64_t)id);
    }

    gic_eoi(id);

    if (id == 30) {
      yield();
    }
    return;
  }

  uint64_t esr = READ_SYSREG(esr_el1);
  uint64_t ec = (esr >> 26) & 0x3F;

  if (idx == 8) {
    if (ec == 0x15) {
      syscall_dispatch(tf);
      return;
    }
    user_fault(tf, esr);
  }

  kprintf("EXCEPTION: slot %lu\n", idx);
  kprintf("ESR_EL1: 0x%lX\n", esr);
  kprintf("DFSC: 0x%lX\n", esr & 0x3F);
  kprintf("CLASS: 0x%lx (%s)\n", ec, esr_class_name(ec));
  kprintf("ELR_EL1: 0x%lX\n", READ_SYSREG(elr_el1));
  kprintf("FAR_EL1: 0x%lX\n", READ_SYSREG(far_el1));
  kprintf("frame ELR: 0x%lX\n", tf->elr);
  kprintf("frame SPSR: 0x%lX\n", tf->spsr);

  if (ec == 0x15) {
    return;
  }

  panic("Unhandled exception");
}
