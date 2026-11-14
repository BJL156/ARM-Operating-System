#include "syscall.h"
#include "kprintf.h"
#include "memmap.h"
#include "sched.h"
#include "sysreg.h"
#include "task.h"
#include "uart.h"

extern char __user_start[], __user_end[];

static int in_range(uint64_t addr, uint64_t len, uint64_t lo, uint64_t hi) {
  return addr >= lo && addr <= hi && len <= hi - addr;
}

static int user_range_ok(uint64_t addr, uint64_t len) {
  return in_range(addr, len, (uint64_t)__user_start, (uint64_t)__user_end) ||
         in_range(addr, len, USER_START, USER_START + USER_SIZE);
}

static long sys_write(const char *buf, uint64_t len) {
  if (!user_range_ok((uint64_t)buf, len)) {
    kprintf("Syscall: bad user pointer from %s.\n", sched_current()->name);
    return -1;
  }
  for (uint64_t i = 0; i < len; i++) {
    uart_putc(buf[i]);
  }
  return (long)len;
}

void syscall_dispatch(TrapFrame *trap_frame) {
  uint64_t a0 = trap_frame->x[0];
  uint64_t a1 = trap_frame->x[1];
  long ret = -1;

  switch (trap_frame->x[8]) {
    case SYS_WRITE:
      ret = sys_write((const char *)a0, a1);
      break;
    case SYS_SLEEP:
      task_sleep_ms(a0);
      ret = 0;
      break;
    case SYS_EXIT:
      kprintf("%s: exit(%ld)\n", sched_current()->name, (long)a0);
      task_exit();
  }

  trap_frame->x[0] = (uint64_t)ret;
}

void user_fault(TrapFrame *trap_frame, uint64_t esr) {
  kprintf("User fault in %s: class 0x%lx, ESR 0x%lx\n",
          sched_current()->name, (esr >> 26) & 0x3F, esr);
  kprintf("  ELR 0x%lx, FAR 0x%lx\n", trap_frame->elr, READ_SYSREG(far_el1));
  task_exit();
}
