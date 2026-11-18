#include "ulib.h"
#include "usys.h"

#define LINE_MAX 64
#define MAX_ARGS 8

static const char *spawn_error(long e) {
  switch (e) {
    case E_NOENT: return "Unknown program";
    case E_2BIG:  return "Argument list too long";
    case E_NOMEM: return "Out of memory";
    default:      return "Spawn failed";
  }
}

static int tokenize(char *line, char *argv[], int max) {
  int argc = 0;
  char *p = line;

  while (*p) {
    while (*p == ' ') {
      *p++ = '\0';
    }

    if (*p == '\0') {
      break;
    }

    if (argc == max - 1) {
      return -1;
    }

    argv[argc++] = p;
    while (*p && *p != ' ') {
      p++;
    }
  }

  argv[argc] = NULL;
  return argc;
}

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

    char *argv[MAX_ARGS + 1];
    int argc = tokenize(line, argv, MAX_ARGS + 1);
    if (argc < 0) {
      u_puts("Too many arguments.\r\n");
      continue;
    }

    if (argc == 0) {
      continue;
    }

    long pid = usys_spawn(argv);
    if (pid < 0) {
      u_puts(spawn_error(pid));
      u_puts(".\r\n");

      continue;
    }

    long status = usys_wait(pid);
    if (status < 0) {
      u_puts("wait failed.\r\n");
    } else if (status != 0) {
      u_puts("Exit status ");
      u_putnum((unsigned long)status);
      u_puts(".\r\n");
    }
  }
}
