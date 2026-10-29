#include "gic.h"
#include "kprintf.h"
#include "mem.h"
#include "mmu.h"
#include "pmm.h"
#include "sysreg.h"
#include "timer.h"

#include <stddef.h>
#include <stdint.h>

#define RAM_BASE 0x40000000UL
#define RAM_SIZE (128UL * 1024 * 1024)
#define RAM_END  (RAM_BASE + RAM_SIZE)

#define TIMER_IRQ 30

extern char _start[];
extern char __kernel_end[];

void kernel_main(void) {
  kprintf("Hello from ARM Operating System.\n");
  kprintf("Current EL: %lu\n", READ_SYSREG(CurrentEL) >> 2);

  kprintf("Kernel start: %p\n", (void *)_start);
  kprintf("Kernel end:   %p\n", (void *)__kernel_end);
  kprintf("RAM end:      %p\n", (void *)RAM_END);

  pmm_init((uintptr_t)__kernel_end, RAM_END);
  kprintf("Free pages:   %lu\n", pmm_free_count());

  mmu_init();

  extern char __text_end[];
  *(volatile uint32_t *)(__text_end - 4) = 0;

  gic_init();
  gic_enable_irq(TIMER_IRQ);
  timer_start(timer_freq());
  asm volatile("msr daifclr, #2");

  while (1) {
    asm volatile("wfi");
  }
}
