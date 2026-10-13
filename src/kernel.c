#include "gic.h"
#include "kprintf.h"
#include "timer.h"

#include <stdint.h>

#define RAM_BASE 0x40000000UL
#define RAM_SIZE (128UL * 1024 * 1024)
#define RAM_END  (RAM_BASE + RAM_SIZE)

#define PAGE_SIZE 4096UL

extern char __kernel_end[];
extern char _start[];

void kernel_main(void) {
  uint64_t kernel_start = (uint64_t)_start;
  uint64_t kernel_end = (uint64_t)__kernel_end;
  uint64_t ram_end = RAM_END;

  uint64_t free_pages = (ram_end - kernel_end) / PAGE_SIZE;

  kprintf("Hello from ARM Operating System.\n");

  kprintf("Kernel start: %lx\n", (unsigned long)kernel_start);
  kprintf("Kernel end:   %lx\n", (unsigned long)kernel_end);
  kprintf("RAM end:      %lx\n", (unsigned long)ram_end);
  kprintf("Free pages:   %lu\n", (unsigned long)free_pages);

  gic_init();
  gic_enable_irq(30);
  timer_start(timer_freq());
  asm volatile("msr daifclr, #2");

  while (1) {
    asm volatile("wfi");
  }
}
