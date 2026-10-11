#include "panic.h"

#include "kprintf.h"

void panic(const char *msg) {
  kprintf("PANIC: %s\n", msg);

  while (1) {
    asm volatile("wfe");
  }
}
