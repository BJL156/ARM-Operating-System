#include "mmu.h"

#include "kprintf.h"
#include "pmm.h"
#include "sysreg.h"

#include <stdint.h>

void mmu_enable(void) {
  asm volatile("dsb ish");
  asm volatile("tlbi vmalle1");
  asm volatile("dsb ish");
  asm volatile("isb");

  uint64_t sctlr = READ_SYSREG(sctlr_el1);
  sctlr |= (1ULL << 0);
  WRITE_SYSREG(sctlr_el1, sctlr);
  asm volatile("isb");
}

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

  WRITE_SYSREG(mair_el1, MAIR_VALUE);
  WRITE_SYSREG(tcr_el1, TCR_VALUE);
  WRITE_SYSREG(ttbr0_el1, (uint64_t)l1);
  asm volatile("isb");

  kprintf("MAIR_EL1:  0x%lx\n", READ_SYSREG(mair_el1));
  kprintf("TCR_EL1:   0x%lx\n", READ_SYSREG(tcr_el1));
  kprintf("TTBR0_EL1: 0x%lx\n", READ_SYSREG(ttbr0_el1));

  mmu_enable();
}
