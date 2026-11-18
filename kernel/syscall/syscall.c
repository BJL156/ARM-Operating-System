#include "syscall.h"

#include "aspace.h"
#include "kprintf.h"
#include "irq.h"
#include "memmap.h"
#include "sched.h"
#include "syscall_abi.h"
#include "sysreg.h"
#include "task.h"
#include "process.h"
#include "progs.h"
#include "uart.h"

#define MAX_ARGS 8
#define ARG_LEN  64

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

static long copy_user_str(char *dst, int max, const char *src) {
  ASpace *aspace = sched_current()->aspace;

  for (int i = 0; i < max; i++) {
    if (!aspace_user_ok(aspace, (uint64_t)src + i, 1, 0)) {
      return -1;
    }

    dst[i] = src[i];
    if (dst[i] == '\0') {
      return (long)i;
    }
  }

  return E_2BIG;
}

static long sys_spawn(uint64_t uargv) {
  char args[MAX_ARGS][ARG_LEN];
  const char *argv[MAX_ARGS];
  int argc = 0;
  ASpace *aspace = sched_current()->aspace;

  while (1) {
    uint64_t slot = uargv + (uint64_t)argc * 8;
    if (!aspace_user_ok(aspace, slot, 8, 0)) {
      return E_FAULT;
    }

    uint64_t ptr = *(uint64_t *)slot;
    if (ptr == 0) {
      break;
    }

    if (argc == MAX_ARGS) {
      return E_2BIG;
    }

    int n = copy_user_str(args[argc], ARG_LEN, (const char *)ptr) < 0;
    if (n < 0) {
      return n;
    }

    argv[argc] = args[argc];
    argc++;
  }

  if (argc == 0) {
    kprintf("Syscall: empty argv.\n");
    return E_NOENT;
  }

  const Prog *prog = prog_find(argv[0]);
  if (prog == NULL) {
    kprintf("Syscall: no program '%s' (argc %d).\n", argv[0], argc);
    for (int i = 0; i < 8; i++) {
      kprintf(" %x", (unsigned)(unsigned char)argv[0][i]);
    }
    kprintf("\n");

    return E_NOENT;
  }

  Task *task = process_create(prog->start, prog->end - prog->start, prog->name, argc, argv);
  if (task == NULL) {
    return E_NOMEM;
  }

  task->parent_id = sched_current()->id;

  uint64_t id = task->id;
  sched_add(task);

  return (long)id;
}

static long sys_wait(uint64_t pid) {
  Task *me = sched_current();

  long ret = -1;
  uint64_t flags = irq_save();

  Task *task = sched_find(pid);
  if (task != NULL && task->parent_id == me->id && !task->collected) {
    while (task->state != TASK_DEAD) {
      sched_block(task);
    }

    ret = task->exit_code;
    task->collected = 1;
  }

  irq_restore(flags);

  return ret;
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
      sched_current()->exit_code = (int)(a0 & 0xFF);
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
    case SYS_SPAWN: {
      ret = sys_spawn(a0);
      break;
    }
    case SYS_WAIT: {
      ret = sys_wait(a0);
      break;
    }
  }

  trap_frame->x[0] = (uint64_t)ret;
}

void user_fault(TrapFrame *trap_frame, uint64_t esr) {
  kprintf("User fault in %s: class 0x%lx, ESR 0x%lx\n", sched_current()->name, (esr >> 26) & 0x3F, esr);
  kprintf("  ELR 0x%lx, FAR 0x%lx\n", trap_frame->elr, READ_SYSREG(far_el1));

  sched_current()->exit_code = 139;
  task_exit();
}
