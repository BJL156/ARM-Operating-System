#include "ulib.h"
#include "usys.h"

void _start(long argc, char **argv) {
  for (long i = 1; i < argc; i++) {
    if (i > 1) {
      u_puts(" ");
    }

    u_puts(argv[i]);
  }

  u_puts("\r\n");

  usys_exit(0);
}
