#include "ulib.h"

void _start(long argc, char **argv) {
  long code = (argc > 1) ? u_atoi(argv[1]) : 0;

  u_puts("Exiting with status ");
  u_putnum((unsigned long)code);
  u_puts(".\r\n");

  usys_exit(code);
}
