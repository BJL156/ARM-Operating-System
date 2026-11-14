#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

void kheap_init(uintptr_t start, size_t size);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
