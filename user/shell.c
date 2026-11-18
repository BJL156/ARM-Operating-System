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

enum {
  KEY_NONE = 256, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT,
  KEY_HOME, KEY_END, KEY_DELETE
};

static int read_byte(void) {
  char c;
  long n = usys_read(&c, 1);
  if (n < 0) {
    u_puts("read failed\r\n");
    usys_exit(1);
  }
  return (n == 1) ? (unsigned char)c : -1;
}

static int read_key(void) {
  int b = read_byte();
  if (b == 0x01) return KEY_HOME;
  if (b == 0x05) return KEY_END;
  if (b != 0x1b) return b;

  int c = read_byte();
  if (c == 'O') {
    c = read_byte();
    switch (c) {
      case 'A': return KEY_UP;
      case 'B': return KEY_DOWN;
      case 'C': return KEY_RIGHT;
      case 'D': return KEY_LEFT;
      case 'H': return KEY_HOME;
      case 'F': return KEY_END;
      default:  return KEY_NONE;
    }
  }

  if (c != '[') {
    return KEY_NONE;
  }

  int num = 0;
  int after_semicolon = 0;
  while (1) {
    c = read_byte();
    if (c >= '0' && c <= '9') {
      if (!after_semicolon) num = num * 10 + (c - '0');
    } else if (c == ';') {
      after_semicolon = 1;
    } else if (c >= 0x40 && c <= 0x7e) {
      break;
    }
  }

  switch (c) {
    case 'A': return KEY_UP;
    case 'B': return KEY_DOWN;
    case 'C': return KEY_RIGHT;
    case 'D': return KEY_LEFT;
    case 'H': return KEY_HOME;
    case 'F': return KEY_END;
    case '~':
      if (num == 3) return KEY_DELETE;
      if (num == 1 || num == 7) return KEY_HOME;
      if (num == 4 || num == 8) return KEY_END;
      return KEY_NONE;
    default:  return KEY_NONE;
  }
}

static void cursor_left(int n) {
  if (n > 0) {
    u_puts("\x1b[");
    u_putnum((unsigned long)n);
    u_puts("D");
  }
}

static void cursor_right(int n) {
  if (n > 0) {
    u_puts("\x1b[");
    u_putnum((unsigned long)n);
    u_puts("C");
  }
}

static void redraw(const char *buf, int from, int len, int cursor, int blank) {
  if (len > from) {
    usys_write(buf + from, (size_t)(len - from));
  }

  if (blank) {
    u_puts(" ");
  }

  cursor_left((len - cursor) + (blank ? 1 : 0));
}

static int read_line(char *buf, int max) {
  int len = 0;
  int pos = 0;

  for (;;) {
    int k = read_key();

    if (k == '\r' || k == '\n') {
      u_puts("\r\n");
      buf[len] = '\0';
      return len;
    }
    if (k == 0x04 && len == 0) {
      return -1;
    }

    switch (k) {
      case KEY_LEFT: {
        if (pos > 0) {
          cursor_left(1);
          pos--;
        }

        break;
      }
      case KEY_RIGHT: {
        if (pos < len) {
          cursor_right(1);
          pos++;
        }

        break;
      }
      case KEY_HOME: {
        cursor_left(pos);
        pos = 0;

        break;
      }
      case KEY_END: {
        cursor_right(len - pos);
        pos = len;

        break;
      }
      case 0x7f:
      case 0x08: {
        if (pos > 0) {
          for (int i = pos - 1; i < len - 1; i++) {
            buf[i] = buf[i + 1];
          }

          len--;
          pos--;

          cursor_left(1);
          redraw(buf, pos, len, pos, 1);
        }

        break;
      }
      case KEY_DELETE: {
        if (pos < len) {
          for (int i = pos; i < len - 1; i++) {
            buf[i] = buf[i + 1];
          }

          len--;

          redraw(buf, pos, len, pos, 1);
        }

        break;
      }
      default: {
        if (k >= 0x20 && k <= 0x7e && len < max - 1) {
          for (int i = len; i > pos; i--) {
            buf[i] = buf[i - 1];
          }

          buf[pos++] = (char)k;

          len++;

          redraw(buf, pos - 1, len, pos, 0);
        }

        break;
      }
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
      u_puts(" (");
      u_putnum((unsigned long)-pid);
      u_puts(")\r\n");

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
