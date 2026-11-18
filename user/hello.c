#include "ulib.h"

void _start(void) {
  for (int i = 0; i < 3; i++) {
    u_puts("hello from pid ");
    u_putnum(usys_getpid());
    u_puts("\r\n");
    usys_sleep(500);
  }

  usys_exit(0);
}
