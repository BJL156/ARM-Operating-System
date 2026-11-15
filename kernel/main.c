#include "gic.h"
#include "kheap.h"
#include "kprintf.h"
#include "memmap.h"
#include "mmu.h"
#include "pmm.h"
#include "process.h"
#include "sched.h"
#include "sysreg.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

#include <stddef.h>
#include <stdint.h>

#define TIMER_IRQ 30

extern char _start[];
extern char __kernel_end[];
extern char hello_elf_start[], hello_elf_end[];

static void heartbeat(void *arg) {
  (void)arg;
  for (uint64_t i = 0; ; i++) {
    kprintf("heartbeat %lu, free pages %lu\n", i, pmm_free_count());
    task_sleep_ms(1000);
  }
}

void kernel_main(void) {
  kprintf("Hello from ARM Operating System.\n");
  kprintf("Current EL: %lu\n", READ_SYSREG(CurrentEL) >> 2);

  kprintf("Kernel start: %p\n", (void *)_start);
  kprintf("Kernel end:   %p\n", (void *)__kernel_end);
  kprintf("Heap:         %p - %p\n", (void *)HEAP_START, (void *)RAM_END);

  pmm_init((uintptr_t)__kernel_end, HEAP_START);
  kprintf("Free pages:   %lu\n", pmm_free_count());
  kheap_init(HEAP_START, HEAP_SIZE);
  mmu_init();

  kprintf("baseline free pages: %lu\n", pmm_free_count());

  sched_init();
  size_t hello_size = hello_elf_end - hello_elf_start;
  // sched_add(process_create(hello_elf_start, hello_size, "p1"));
  // sched_add(process_create(hello_elf_start, hello_size, "p2"));
  // sched_add(task_create(heartbeat, NULL, "beat"));
  extern char echo_elf_start[], echo_elf_end[];
  sched_add(process_create(echo_elf_start, echo_elf_end - echo_elf_start, "echo"));

  gic_init();
  uart_rx_init();
  gic_route_to_cpu0(UART_IRQ);
  gic_enable_irq(UART_IRQ);
  gic_enable_irq(TIMER_IRQ);
  timer_start(timer_freq() / TICK_HZ);
  asm volatile("msr daifclr, #2");

  while (1) {
    sched_reap();
    asm volatile("wfi");
  }
}
