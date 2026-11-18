#ifndef ULIB_H
#define ULIB_H

#include "usys.h"

static inline int u_strlen(const char *s) {
  const char *s_end = s;

  while (*s_end) {
    s_end++;
  }

  return s_end - s;
}

static inline void u_puts(const char *s) {
  usys_write(s, u_strlen(s));
}

static inline void u_putnum(unsigned long num) {
  char buf[20];

  int i = 0;
  do {
    buf[i++] = '0' + num % 10;
    num /= 10;
  } while (num);

  while (i > 0) {
    usys_write(&buf[--i], 1);
  }
}

static inline long u_atoi(const char *s) {
  long i = 0;

  while (*s >= '0' && *s <= '9') {
    i = i * 10 + (*s - '0');
    s++;
  }

  return i;
}

#endif
