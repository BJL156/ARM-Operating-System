#include "gic.h"
#include "kprintf.h"
#include "timer.h"

#include <stdint.h>

void kernel_main(void) {
  kprintf("Hello from ARM Operating System.\n");

  gic_init();
  gic_enable_irq(30);
  timer_start(timer_freq());
  asm volatile("msr daifclr, #2");

  while (1) {
    asm volatile("wfi");
  }
}
