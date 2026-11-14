#ifndef SCHED_H
#define SCHED_H

#include "task.h"

void sched_init(void);
void sched_add(Task *task);
void yield(void);

Task *sched_current(void);

#endif
