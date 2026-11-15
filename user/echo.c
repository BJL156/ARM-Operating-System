#include "usys.h"

void _start(void) {
  char c;
  for (;;) {
    if (usys_read(&c, 1) != 1) {
      continue;
    }

    if (c == '\r' || c == '\n') {
      usys_write("\r\n", 2);
    } else if (c == 0x7f) {
      usys_write("\b \b", 3);
    } else {
      usys_write(&c, 1);
    }
  }
}
