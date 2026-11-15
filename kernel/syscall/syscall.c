#include "syscall.h"
#include "kprintf.h"
#include "memmap.h"
#include "sched.h"
#include "sysreg.h"
#include "task.h"
#include "uart.h"

static long sys_write(const char *buf, uint64_t len) {
  if (len > 4096) {
    len = 4096;
  }

  if (!aspace_user_ok(sched_current()->aspace, (uint64_t)buf, len, 0)) {
    kprintf("Syscall: bad user pointer from %s.\n", sched_current()->name);
    return -1;
  }

  for (uint64_t i = 0; i < len; i++) {
    uart_putc(buf[i]);
  }

  return (long)len;
}

static long sys_read(char *buf, uint64_t len) {
  if (len > 4096) {
    len = 4096;
  }

  if (len == 0) {
    return 0;
  }

  if (!aspace_user_ok(sched_current()->aspace, (uint64_t)buf, len, 1)) {
    kprintf("Syscall: bad user pointer from %s.\n", sched_current()->name);
    return -1;
  }

  buf[0] = (char)uart_getc();
  uint64_t n = 1;
  while (n < len) {
    int c = uart_getc_nb();
    if (c < 0) {
      break;
    }
    buf[n++] = (char)c;
  }
  return (long)n;
}

void syscall_dispatch(TrapFrame *trap_frame) {
  uint64_t a0 = trap_frame->x[0];
  uint64_t a1 = trap_frame->x[1];
  long ret = -1;

  switch (trap_frame->x[8]) {
    case SYS_WRITE: {
      ret = sys_write((const char *)a0, a1);
      break;
    }
    case SYS_SLEEP: {
      task_sleep_ms(a0);
      ret = 0;
      break;
    }
    case SYS_EXIT: {
      kprintf("%s: exit(%ld)\n", sched_current()->name, (long)a0);
      task_exit();
    }
    case SYS_GETPID: {
      ret = (long)sched_current()->id;
      break;
    }
    case SYS_READ: {
      ret = sys_read((char *)a0, a1);
      break;
    }
  }

  trap_frame->x[0] = (uint64_t)ret;
}

void user_fault(TrapFrame *trap_frame, uint64_t esr) {
  kprintf("User fault in %s: class 0x%lx, ESR 0x%lx\n",
          sched_current()->name, (esr >> 26) & 0x3F, esr);
  kprintf("  ELR 0x%lx, FAR 0x%lx\n", trap_frame->elr, READ_SYSREG(far_el1));
  task_exit();
}
