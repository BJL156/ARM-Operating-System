#ifndef ASPACE_H
#define ASPACE_H

#include <stdint.h>

#define USER_VA_BASE  0x100000000UL
#define USER_VA_LIMIT (USER_VA_BASE + 0x40000000UL)

typedef struct {
  uint64_t *l1;
} ASpace;

ASpace *aspace_kernel(void);
ASpace *aspace_create(void);
void aspace_map(ASpace *as, uint64_t va, uint64_t pa, uint64_t flags);
void aspace_switch(ASpace *as);

int aspace_user_ok(ASpace *as, uint64_t va, uint64_t len, int write);

void aspace_destroy(ASpace *aspace);

#endif
