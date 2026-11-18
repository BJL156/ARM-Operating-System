#include "progs.h"

#include <stddef.h>

extern char hello_elf_start[], hello_elf_end[];
extern char echo_elf_start[], echo_elf_end[];
extern char shell_elf_start[], shell_elf_end[];
extern char fail_elf_start[], fail_elf_end[];
extern char crash_elf_start[], crash_elf_end[];

static const Prog progs[] = {
  { "hello", hello_elf_start, hello_elf_end },
  { "echo",  echo_elf_start,  echo_elf_end  },
  { "sh",    shell_elf_start, shell_elf_end },
  { "fail",  fail_elf_start,  fail_elf_end  },
  { "crash", crash_elf_start, crash_elf_end },
};

static int str_equal(const char *a, const char *b) {
  while (*a == *b) {
    if (*a == '\0') {
      return 1;
    }

    a++;
    b++;
  }

  return 0;
}

const Prog *prog_find(const char *name) {
  int n = sizeof(progs) / sizeof(progs[0]);

  for (int i = 0; i < n; i++) {
    if (str_equal(name, progs[i].name)) {
      return &progs[i];
    }
  }

  return NULL;
}
