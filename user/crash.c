#include "ulib.h"

void _start(void) {
  u_puts("Going to crash by modifying 0x1000.\r\n");
  *(volatile long *)0x1000 = 1;
  usys_exit(0);
}
