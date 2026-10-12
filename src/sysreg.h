#ifndef SYSREG_H
#define SYSREG_H

#include <stdint.h>

#define READ_SYSREG(name) ({ \
    uint64_t _v; \
    asm volatile("mrs %0, " #name : "=r"(_v)); \
    _v; \
})

#define WRITE_SYSREG(name, value) ({ \
    asm volatile("msr " #name ", %0" :: "r"((uint64_t)(value))); \
})

#endif
