#include "mmu.h"

#include "kprintf.h"
#include "panic.h"
#include "pmm.h"
#include "sysreg.h"

#include <stdint.h>
#include <stddef.h>

extern char __text_start[], __text_end[];
extern char __rodata_start[], __rodata_end[];
extern char __data_start[], __kernel_end[];

#define RAM_BASE 0x40000000UL
#define RAM_SIZE (128UL * 1024 * 1024)
#define RAM_END  (RAM_BASE + RAM_SIZE)

#define GICD_PHYS 0x08000000UL
#define GICC_PHYS 0x08010000UL
#define UART_PHYS 0x09000000UL

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

static void show_entry(const char *name, uint64_t l1[], uint64_t va) {
  uint64_t *e = walk(l1, va, 0);
  if (e == NULL) {
    kprintf("%s (0x%lx): not mapped\n", name, va);
  } else {
    kprintf("%s (0x%lx): 0x%lx\n", name, va, *e);
  }
}

void mmu_init(void) {
  uint64_t *l1 = pmm_alloc_zeroed();
  if (l1 == NULL) {
    panic("MMU: out of memory for L1 table.");
  }

  map_range(l1, (uint64_t)__text_start, (uint64_t)__text_start,
            __text_end - __text_start, MAP_RX);
  map_range(l1, (uint64_t)__rodata_start, (uint64_t)__rodata_start,
            __rodata_end - __rodata_start, MAP_RO);

  map_range(l1, (uint64_t)__data_start, (uint64_t)__data_start,
            RAM_END - (uint64_t)__data_start, MAP_RW);

  map_range(l1, GICD_PHYS, GICD_PHYS, 0x1000, MAP_DEV);
  map_range(l1, GICC_PHYS, GICC_PHYS, 0x2000, MAP_DEV);
  map_range(l1, UART_PHYS, UART_PHYS, 0x1000, MAP_DEV);

  show_entry("text  ", l1, (uint64_t)__text_start);
  show_entry("rodata", l1, (uint64_t)__rodata_start);
  show_entry("data  ", l1, (uint64_t)__data_start);
  show_entry("uart  ", l1, UART_PHYS);

  WRITE_SYSREG(mair_el1, MAIR_VALUE);
  WRITE_SYSREG(tcr_el1, TCR_VALUE);
  WRITE_SYSREG(ttbr0_el1, (uint64_t)l1);
  asm volatile("isb");

  mmu_enable();
}
