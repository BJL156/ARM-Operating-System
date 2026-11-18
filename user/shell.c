#include "ulib.h"
#include "usys.h"

#define LINE_MAX 64

static int read_line(char *buf, int max) {
  int len = 0;

  for (;;) {
    char c;

    if (usys_read(&c, 1) != 1) {
      continue;
    }

    if (c == '\r' || c == '\n') {
      u_puts("\r\n");
      buf[len] = '\0';
      return len;
    }

    if (c == 0x04 && len == 0) {
      return -1;
    }

    if (c == 0x7f) {
      if (len > 0) {
        len--;
        u_puts("\b \b");
      }
      continue;
    }

    if (len < max - 1) {
      buf[len++] = c;
      usys_write(&c, 1);
    }
  }
}

void _start(void) {
  char line[LINE_MAX];

  for (;;) {
    u_puts("> ");
    int len = read_line(line, LINE_MAX);

    if (len < 0) {
      u_puts("Adios.\r\n");
      usys_exit(0);
    }
    if (len == 0) {
      continue;
    }

    long pid = usys_spawn(line);
    if (pid < 0) {
      u_puts("Unknown program.\r\n");
      continue;
    }
    usys_wait(pid);
  }
}
