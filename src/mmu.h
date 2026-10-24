#ifndef MMU_H
#define MMU_H

#include <stdint.h>

#define PTE_TYPE_MASK  0x3ULL
#define PTE_BLOCK      0x1ULL
#define PTE_VALID     (1UL << 0)
#define PTE_TABLE     (3UL << 0)
#define PTE_PAGE      (3UL << 0)
#define PTE_AP_RO     (1UL << 7)
#define PTE_AP_EL0    (1UL << 6)
#define PTE_PXN       (1UL << 53)
#define PTE_UXN       (1UL << 54)

#define PTE_ATTR_SHIFT 2
#define PTE_ATTR_MASK  (0x7ULL << PTE_ATTR_SHIFT)

#define PTE_AP_SHIFT   6
#define PTE_AP_MASK    (0x3ULL << PTE_AP_SHIFT)
#define PTE_AP_EL1_RW  (0x0ULL << PTE_AP_SHIFT)

#define PTE_SH_SHIFT   8
#define PTE_SH_MASK    (0x3ULL << PTE_SH_SHIFT)
#define PTE_SH_INNER   (0x3ULL << PTE_SH_SHIFT)

#define PTE_AF         (1ULL << 10)

#define PTE_ADDR_MASK  0x0000FFFFFFFFF000ULL

#define PTE_ATTR(idx) ((uint64_t)(idx) << 2)

#define MAP_RX  (PTE_AF | PTE_SH_INNER | PTE_ATTR(MAIR_IDX_NORMAL) | \
                PTE_AP_RO | PTE_UXN)

#define MAP_RO  (PTE_AF | PTE_SH_INNER | PTE_ATTR(MAIR_IDX_NORMAL) | \
                PTE_AP_RO | PTE_PXN | PTE_UXN)

#define MAP_RW  (PTE_AF | PTE_SH_INNER | PTE_ATTR(MAIR_IDX_NORMAL) | \
                PTE_PXN | PTE_UXN)

#define MAP_DEV (PTE_AF | PTE_ATTR(MAIR_IDX_DEVICE) | \
                PTE_PXN | PTE_UXN)

#define MAIR_IDX_DEVICE 0
#define MAIR_IDX_NORMAL 1
#define MAIR_ATTR_DEVICE_nGnRnE 0x00UL
#define MAIR_ATTR_NORMAL_WB     0xFFUL
#define MAIR_VALUE              ((MAIR_ATTR_DEVICE_nGnRnE << (8 * MAIR_IDX_DEVICE)) | \
                                (MAIR_ATTR_NORMAL_WB     << (8 * MAIR_IDX_NORMAL)))

#define TCR_T0SZ(n)   ((uint64_t)(n) << 0)
#define TCR_IRGN0_WB  (1UL << 8)
#define TCR_ORGN0_WB  (1UL << 10)
#define TCR_SH0_INNER (3UL << 12)
#define TCR_TG0_4K    (0UL << 14)
#define TCR_TG1_4K    (2UL << 30)
#define TCR_EPD1      (1UL << 23)
#define TCR_IPS_40BIT (2UL << 32)

#define TCR_VALUE (TCR_T0SZ(25) | TCR_IRGN0_WB | TCR_ORGN0_WB | \
                  TCR_SH0_INNER | TCR_TG0_4K | TCR_TG1_4K |    \
                  TCR_EPD1 | TCR_IPS_40BIT)

void map_page(uint64_t *l1, uint64_t va, uint64_t pa, uint64_t flags);
void map_range(uint64_t *l1, uint64_t va, uint64_t pa, uint64_t size, uint64_t flags);

void mmu_enable(void);
void mmu_init(void);

#endif
