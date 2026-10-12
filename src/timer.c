#include "sysreg.h"

uint64_t timer_freq(void) {
  return READ_SYSREG(cntfrq_el0);
}

uint64_t timer_count(void) {
  return READ_SYSREG(cntpct_el0);
}
