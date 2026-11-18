#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"

Task *process_create(const void *image, size_t size, const char *name, int argc, const char *const *argv);

#endif
