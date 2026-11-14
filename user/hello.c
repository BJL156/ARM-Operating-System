#include "usys.h"

static char line[] = "pid ?: tick ?\n";

void _start(void) {
  line[4] = '0' + usys_getpid();
  for (int i = 0; i < 5; i++) {
    line[12] = '0' + i;
    usys_write(line, sizeof(line) - 1);
    usys_sleep(500);
  }
  usys_exit(0);
}
