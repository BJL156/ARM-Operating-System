#include "gic.h"

#define GICD_BASE         0x08000000UL
#define GICD_CTLR         (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER(n) (*(volatile uint32_t *)(GICD_BASE + 0x100 + 4 * (n)))
#define GICD_IPRIORITYR   (*(volatile uint32_t *)(GICD_BASE + 0x400))

#define GICC_BASE 0x08010000UL
#define GICC_CTLR (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR  (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_IAR  (*(volatile uint32_t *)(GICC_BASE + 0x00C))
#define GICC_EOIR (*(volatile uint32_t *)(GICC_BASE + 0x010))

void gic_init(void) {
  GICD_CTLR = 1;
  GICC_PMR = 0xFF;
  GICC_CTLR = 1;
}

void gic_enable_irq(uint32_t id) {
  uint32_t reg = id / 32;
  uint32_t bit = id % 32;


  GICD_ISENABLER(reg) = (1U << bit);
}

uint32_t gic_ack() {
  return GICC_IAR & 0x3FF;
}

void gic_eoi(uint32_t id) {
  GICC_EOIR = id;
}
