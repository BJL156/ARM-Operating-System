#include "gic.h"
#include "kheap.h"
#include "kprintf.h"
#include "mmu.h"
#include "pmm.h"
#include "sysreg.h"
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

  gic_init();
  gic_enable_irq(TIMER_IRQ);
  timer_start(timer_freq());
  asm volatile("msr daifclr, #2");

  void *a = kmalloc(100);
  void *b = kmalloc(50);
  void *c = kmalloc(1);
  kprintf("a=%p b=%p c=%p\n", a, b, c);
  kprintf("kmalloc(0)=%p, kmalloc(too big)=%p\n", kmalloc(0), kmalloc(HEAP_SIZE));

  kfree(b);
  void *b2 = kmalloc(50);
  kprintf("b=%p b2=%p (should match)\n", b, b2);

  kfree(a);
  kfree(b2);
  kfree(c);
  void *big = kmalloc(HEAP_SIZE - 32);
  kprintf("big=%p (should be %p)\n", big, a);

  while (1) {
    asm volatile("wfi");
  }
}
