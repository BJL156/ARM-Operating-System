#ifndef USYS_H
#define USYS_H

#include <stddef.h>

#include "syscall.h"

#define USER_INLINE static inline __attribute__((always_inline))

USER_INLINE long usys_call3(long n, long a, long b, long c) {
  register long x8 asm("x8") = n;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  
  asm volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory");
  
  return x0;
}

USER_INLINE long usys_write(const char *buf, size_t len) {
  return usys_call3(SYS_WRITE, (long)buf, (long)len, 0);
}

USER_INLINE long usys_sleep(long ms) {
  return usys_call3(SYS_SLEEP, ms, 0, 0);
}

USER_INLINE void usys_exit(long code) {
  usys_call3(SYS_EXIT, code, 0, 0);
  
  while (1) {

  }
}

USER_INLINE long usys_getpid(void) {
  return usys_call3(SYS_GETPID, 0, 0, 0);
}

#endif
