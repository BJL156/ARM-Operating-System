#include "mmu.h"

#include "kprintf.h"
#include "pmm.h"

#include <stdint.h>

void mmu_init(void) {
  uint64_t *l1 = (uint64_t *)pmm_alloc_zeroed();
  if (l1 == 0) {
    kprintf("MMU: failed to allocate L1 table.\n");
    return;
  }

  l1[0] = PTE_BLOCK
        | pte_attr(0)
        | PTE_AP_EL1_RW
        | PTE_SH_INNER
        | PTE_AF
        | (0x00000000ULL & PTE_ADDR_MASK);

  l1[1] = PTE_BLOCK
        | pte_attr(1)
        | PTE_AP_EL1_RW
        | PTE_SH_INNER
        | PTE_AF
        | (0x40000000ULL & PTE_ADDR_MASK);

  kprintf("L1[0] = 0x%lX\n", (unsigned long)l1[0]);
  kprintf("L1[1] = 0x%lX\n", (unsigned long)l1[1]);
}
