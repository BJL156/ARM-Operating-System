#ifndef SYSCALL_ABI_H
#define SYSCALL_ABI_H

#define SYS_WRITE  1
#define SYS_SLEEP  2
#define SYS_EXIT   3
#define SYS_GETPID 4
#define SYS_READ   5
#define SYS_SPAWN  6
#define SYS_WAIT   7

#define E_FAULT (-1)
#define E_NOENT (-2)
#define E_2BIG  (-3)
#define E_NOMEM (-4)

#endif
