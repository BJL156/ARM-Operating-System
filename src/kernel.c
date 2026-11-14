#include "gic.h"
#include "kheap.h"
#include "kprintf.h"
#include "mmu.h"
#include "pmm.h"
#include "sched.h"
#include "sysreg.h"
#include "task.h"
#include "timer.h"

#include <stddef.h>
#include <stdint.h>

#define RAM_BASE 0x40000000UL
#define RAM_SIZE (128UL * 1024 * 1024)
#define RAM_END  (RAM_BASE + RAM_SIZE)

#define HEAP_SIZE  (4UL * 1024 * 1024)
#define HEAP_START (RAM_END - HEAP_SIZE)

#define TIMER_IRQ 30

extern char _start[];
extern char __kernel_end[];

static void shortlived(void *arg) {
  kprintf("Short task %lu running.\n", (uint64_t)arg);
}

static void spawner(void *arg) {
  (void)arg;
  for (uint64_t i = 0; i < 6; i++) {
    Task *t = task_create(shortlived, (void *)i, "short");
    kprintf("Created task at %p.\n", (void *)t);
    sched_add(t);
    task_sleep_ms(300);
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

  sched_init();
  sched_add(task_create(spawner, NULL, "spawner"));

  gic_init();
  gic_enable_irq(TIMER_IRQ);
  timer_start(timer_freq() / TICK_HZ);
  asm volatile("msr daifclr, #2");

  while (1) {
    sched_reap();
    asm volatile("wfi");
  }
}
