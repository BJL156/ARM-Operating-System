#include "kprintf.h"

#include <stdint.h>

extern char _start[];

void kernel_main(void) {
  uint64_t el;
  asm volatile("mrs %0, CurrentEL" : "=r"(el));

  kprintf("Current EL: %lu\n", el >> 2);

  register uint64_t r asm("x5") = 0x1234;
  asm volatile("svc #0" : "+r"(r));
  kprintf("x5 after svc: 0x%lX\n", r);

  kprintf("Hello from ARM Operating System.\n");

  while (1) {  
  }
}
