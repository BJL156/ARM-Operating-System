#include "aspace.h"

#include "kheap.h"
#include "mem.h"
#include "mmu.h"
#include "panic.h"
#include "pmm.h"
#include "sysreg.h"

static ASpace kernel_aspace;

ASpace *aspace_kernel(void) {
  kernel_aspace.l1 = mmu_kernel_l1();
  return &kernel_aspace;
}

ASpace *aspace_create(void) {
  ASpace *aspace = kmalloc(sizeof(ASpace));
  uint64_t *l1 = pmm_alloc();

  if (aspace == NULL || l1 == NULL) {
    panic("ASpace: out of memory.");
  }

  memcpy(l1, mmu_kernel_l1(), 4096);
  aspace->l1 = l1;
  return aspace;
}

void aspace_map(ASpace *aspace, uint64_t va, uint64_t pa, uint64_t flags) {
  if (va < USER_VA_BASE) {
    panic("ASpace: user mapping below USER_VA_BASE.");
  }

  map_page(aspace->l1, va, pa, flags);
}

void aspace_switch(ASpace *aspace) {
  asm volatile("dsb ish");
  WRITE_SYSREG(ttbr0_el1, (uint64_t)aspace->l1);
  asm volatile("isb");
  asm volatile("tlbi vmalle1");
  asm volatile("dsb ish");
  asm volatile("isb");
}

int aspace_user_ok(ASpace *aspace, uint64_t va, uint64_t len, int write) {
  if (va < USER_VA_BASE || va > USER_VA_LIMIT || len > USER_VA_LIMIT - va) {
    return 0;
  }

  if (len == 0) {
    return 1;
  }

  uint64_t first = va & ~0xFFFUL;
  uint64_t last  = (va + len - 1) & ~0xFFFUL;

  for (uint64_t p = first; p <= last; p += 4096) {
    uint64_t *e = mmu_lookup(aspace->l1, p);

    if (e == NULL || !(*e & PTE_VALID) || !(*e & PTE_AP_EL0)) {
      return 0;
    }

    if (write && (*e & PTE_AP_RO)) {
      return 0;
    }
  }

  return 1;
}

void aspace_destroy(ASpace *aspace) {
  if (aspace == &kernel_aspace) {
    panic("ASpace: cannot destroy the kernel space.");
  }

  uint64_t lo = (USER_VA_BASE >> 30) & 0x1FF;
  uint64_t hi = ((USER_VA_LIMIT - 1) >> 30) & 0x1FF;

  for (uint64_t i = lo; i <= hi; i++) {
    if (!(aspace->l1[i] & PTE_VALID)) {
      continue;
    }

    uint64_t *l2 = (uint64_t *)(aspace->l1[i] & PTE_ADDR_MASK);

    for (int j = 0; j < 512; j++) {
      if (!(l2[j] & PTE_VALID)) {
        continue;
      }

      uint64_t *l3 = (uint64_t *)(l2[j] & PTE_ADDR_MASK);

      for (int k = 0; k < 512; k++) {
        if (l3[k] & PTE_VALID) {
          pmm_free((void *)(l3[k] & PTE_ADDR_MASK));
        }
      }

      pmm_free(l3);
    }
    pmm_free(l2);
  }

  pmm_free(aspace->l1);
  kfree(aspace);
}
