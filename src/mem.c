#include "mem.h"

void *memset(void *dst, int value, size_t n) {
  unsigned char *p = (unsigned char *)dst;

  for (size_t i = 0; i < n; i++) {
    p[i] = (unsigned char)value;
  }

  return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  for (size_t i = 0; i < n; i++) {
    d[i] = s[i];
  }

  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  if (d == s || n == 0) {
    return dst;
  }

  if (d < s) {
    for (size_t i = 0; i < n; i++) {
      d[i] = s[i];
    }
  } else {
    for (size_t i = n; i > 0; i--) {
      d[i - 1] = s[i - 1];
    }
  }

  return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *p = (const unsigned char *)a;
  const unsigned char *q = (const unsigned char *)b;

  for (size_t i = 0; i < n; i++) {
    if (p[i] != q[i]) {
      return (int)p[i] - (int)q[i];
    }
  }

  return 0;
}
