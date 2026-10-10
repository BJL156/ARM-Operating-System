#include "kprintf.h"
#include "uart.h"

extern char _start[];

void kernel_main(void) {
  uint64_t el;
  asm volatile("mrs %0, CurrentEL" : "=r"(el));

  kprintf("Current EL: %lu\n", el >> 2);

  kprintf("Hello from ARM Operating System.\n");

  while (1) {  
  }
}
