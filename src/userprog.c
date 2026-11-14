#include "usys.h"
#include <stdint.h>

static const char msg[] __attribute__((section(".user.rodata"))) = "hello from EL0\n";

__attribute__((section(".user.text")))
void user_hello(void) {
  for (int i = 0; i < 3; i++) {
    usys_write(msg, sizeof(msg) - 1);
    usys_sleep(500);
  }
  usys_exit(0);
}

__attribute__((section(".user.text")))
void user_bad(void) {
  usys_write((const char *)0x47c00000, 32);
  volatile uint64_t *p = (volatile uint64_t *)0x47c00000;
  uint64_t v = *p;
  (void)v;
  usys_exit(0);
}
