#ifndef MMU_H
#define MMU_H

#define PTE_TYPE_MASK  0x3ULL
#define PTE_BLOCK      0x1ULL
#define PTE_TABLE      0x3ULL

#define PTE_ATTR_SHIFT 2
#define PTE_ATTR_MASK  (0x7ULL << PTE_ATTR_SHIFT)
#define pte_attr(n)    (((uint64_t)(n) & 0x7ULL) << PTE_ATTR_SHIFT)

#define PTE_AP_SHIFT   6
#define PTE_AP_MASK    (0x3ULL << PTE_AP_SHIFT)
#define PTE_AP_EL1_RW  (0x0ULL << PTE_AP_SHIFT)

#define PTE_SH_SHIFT   8
#define PTE_SH_MASK    (0x3ULL << PTE_SH_SHIFT)
#define PTE_SH_INNER   (0x3ULL << PTE_SH_SHIFT)

#define PTE_AF         (1ULL << 10)

#define PTE_ADDR_MASK  0x0000FFFFC0000000ULL
#define PTE_ADDR_SHIFT 30

#endif
