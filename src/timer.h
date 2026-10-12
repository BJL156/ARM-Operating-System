#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

uint64_t timer_freq(void);
uint64_t timer_count(void);
void timer_start(uint64_t ticks);

#endif
