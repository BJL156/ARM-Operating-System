#include "kprintf.h"
#include "timer.h"

#include <stdint.h>

extern char _start[];

void kernel_main(void) {
  uint64_t el;
  asm volatile("mrs %0, CurrentEL" : "=r"(el));

  kprintf("Current EL: %lu\n", el >> 2);

  uint64_t freq = timer_freq();
  kprintf("Timer frequency: %lu\n", freq);

  uint64_t start = timer_count();
  while (timer_count() - start < timer_freq()) {
  }
  kprintf("One second passed\n");

  uint64_t count = timer_count();
  kprintf("Timer count: %lu\n", count);

  register uint64_t r asm("x5") = 0x1234;
  asm volatile("svc #0" : "+r"(r));
  kprintf("x5 after svc: 0x%lX\n", r);

  kprintf("Hello from ARM Operating System.\n");

  count = timer_count();
  kprintf("Timer count: %lu\n", count);

  while (1) {  
  }
}
