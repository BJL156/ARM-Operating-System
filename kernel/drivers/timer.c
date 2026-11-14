#include "sysreg.h"

uint64_t timer_freq(void) {
  return READ_SYSREG(cntfrq_el0);
}

uint64_t timer_count(void) {
  return READ_SYSREG(cntpct_el0);
}

void timer_start(uint64_t ticks) {
  WRITE_SYSREG(cntp_tval_el0, ticks);
  WRITE_SYSREG(cntp_ctl_el0, 1);
}
