#include "fs.h"
#include "fs_format.h"
#include "gic.h"
#include "kheap.h"
#include "kprintf.h"
#include "memmap.h"
#include "mmu.h"
#include "panic.h"
#include "pmm.h"
#include "process.h"
#include "progs.h"
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

  extern char fs_image_start[], fs_image_end[];
  if (fs_init(fs_image_start, fs_image_end - fs_image_start) != 0) {
    panic("Kernel: Bad filesystem image.");
  }

  kprintf("Files:\n");
  for (uint32_t i = 0; i < fs_count(); i++) {
    const FSEntry *entry = fs_entry_at(i);
    kprintf("  %s  %lu bytes\n", entry->name, (uint64_t)entry->size);
  }

  const FSEntry *entry = fs_entry_lookup("hello.txt");
  if (entry != NULL) {
    for (uint32_t i = 0; i < entry->size; i++) {
      uart_putc((char)fs_data(entry)[i]);
    }
  }

  kprintf("Lookup nosuch: %p\n", (void *)fs_entry_lookup("nosuch"));

  sched_init();
  const Prog *sh = prog_find("sh");
  const char *sh_argv[] = { "sh" };
  sched_add(process_create(sh->start, sh->end - sh->start, "sh", 1, sh_argv));

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
