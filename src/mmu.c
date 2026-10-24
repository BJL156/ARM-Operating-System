#include "mmu.h"

#include "kprintf.h"
#include "panic.h"
#include "pmm.h"
#include "sysreg.h"

#include <stdint.h>
#include <stddef.h>

static uint64_t l1_index(uint64_t va) {
  return (va >> 30) & 0x1FF;
}

static uint64_t l2_index(uint64_t va) {
  return (va >> 21) & 0x1FF;
}

static uint64_t l3_index(uint64_t va) {
  return (va >> 12) & 0x1FF;
}

static uint64_t *next_table(uint64_t *table, uint64_t index, int create) {
  uint64_t entry = table[index];

  if (!(entry & PTE_VALID)) {
    if (!create) {
      return NULL;
    }
    uint64_t *new_table = pmm_alloc_zeroed();
    if (new_table == NULL) {
      panic("MMU: Out of memory for page table.");
    }
    table[index] = (uint64_t)new_table | PTE_TABLE;
    return new_table;
  }

  return (uint64_t *)(entry & PTE_ADDR_MASK);
}

static uint64_t *walk(uint64_t *l1, uint64_t va, int create) {
  uint64_t *l2 = next_table(l1, l1_index(va), create);
  if (l2 == NULL) {
    return NULL;
  }

  uint64_t *l3 = next_table(l2, l2_index(va), create);
  if (l3 == NULL) {
    return NULL;
  }

  return &l3[l3_index(va)];
}

void map_page(uint64_t *l1, uint64_t va, uint64_t pa, uint64_t flags) {
  if ((va & 0xFFF) || (pa & 0xFFF)) {
    panic("MMU: address not page-aligned.");
  }

  uint64_t *entry = walk(l1, va, 1);
  if (entry == NULL) {
    panic("MMU: walk failed.");
  }

  if (*entry & PTE_VALID) {
    panic("MMU: page already mapped.");
  }

  *entry = pa | flags | PTE_PAGE;
}

void map_range(uint64_t *l1, uint64_t va, uint64_t pa, uint64_t size, uint64_t flags) {
  if (size & 0xFFF) {
    panic("MMU: size not page-aligned.");
  }

  for (uint64_t off = 0; off < size; off += 4096) {
    map_page(l1, va + off, pa + off, flags);
  }
}

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
    panic("MMU: failed to allocate L1 table.");
    return;
  }

  l1[0] = PTE_BLOCK
        | PTE_ATTR(0)
        | PTE_AP_EL1_RW
        | PTE_SH_INNER
        | PTE_AF
        | (0x00000000ULL & PTE_ADDR_MASK);

  l1[1] = PTE_BLOCK
        | PTE_ATTR(1)
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
