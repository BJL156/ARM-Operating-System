#include "kprintf.h"

#include <stdint.h>

extern char _start[];

void kernel_main(void) {
  uint64_t el;
  asm volatile("mrs %0, CurrentEL" : "=r"(el));

  kprintf("Current EL: %lu\n", el >> 2);

  volatile uint64_t *bad = (volatile uint64_t *)0x41000001;
  uint64_t x = *bad;
  (void)x;

  kprintf("Hello from ARM Operating System.\n");

  while (1) {  
  }
}
