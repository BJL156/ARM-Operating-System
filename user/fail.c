#include "ulib.h"

void _start(void) {
  u_puts("Failing test processing, failing on exit code 7.\r\n");
  usys_exit(7);
}
