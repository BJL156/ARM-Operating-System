#ifndef MEMMAP_H
#define MEMMAP_H

#define RAM_BASE   0x40000000UL
#define RAM_SIZE   (128UL * 1024 * 1024)
#define RAM_END    (RAM_BASE + RAM_SIZE)

#define HEAP_SIZE  (4UL * 1024 * 1024)
#define HEAP_START (RAM_END - HEAP_SIZE)

#define USER_SIZE  (1UL * 1024 * 1024)
#define USER_START (HEAP_START - USER_SIZE)

#endif
