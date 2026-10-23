#ifndef PMM_H
#define PMM_H

#include <stdint.h>

void pmm_init(uintptr_t start, uintptr_t end);
void *pmm_alloc(void);
void *pmm_alloc_zeroed(void);
void pmm_free(void *page);
uint64_t pmm_free_count(void);

#endif
