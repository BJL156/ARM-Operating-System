#include "kheap.h"

#include "irq.h"
#include "panic.h"

typedef struct Block {
  size_t size;
  struct Block *next;
  uint64_t free;
  uint64_t magic;
} Block;

#define BLOCK_MAGIC 0xB10CB10CB10CB10CUL
#define ALIGN16(x)  (((x) + 15UL) & ~15UL)

static Block *heap_head;
static uintptr_t heap_end;

void kheap_init(uintptr_t start, size_t size) {
  if ((start & 15) || size < sizeof(Block) + 16) {
    panic("KHeap: bad region.");
  }

  heap_head = (Block *)start;
  heap_head->size  = size - sizeof(Block);
  heap_head->next  = NULL;
  heap_head->free  = 1;
  heap_head->magic = BLOCK_MAGIC;

  heap_end = start + size;
}

static void split_block(Block *block, size_t size) {
  if (block->size < size + sizeof(Block) + 16) {
    return;
  }

  Block *rest = (Block *)((uint8_t *)block + sizeof(Block) + size);
  rest->size = block->size - size - sizeof(Block);
  rest->next = block->next;
  rest->free = 1;
  rest->magic = BLOCK_MAGIC;

  block->size = size;
  block->next = rest;
}

static void *kmalloc_unlocked(size_t size) {
  if (size == 0) {
    return NULL;
  }

  size = ALIGN16(size);

  for (Block *block = heap_head; block != NULL; block = block->next) {
    if (block->magic != BLOCK_MAGIC) {
      panic("KHeap: corrupted block magic value.");
    }

    if (block->free && block->size >= size) {
      split_block(block, size);
      block->free = 0;

      return (void *)(block + 1);
    }
  }

  return NULL;
}

void *kmalloc(size_t size) {
  uint64_t flags = irq_save();
  void *ptr = kmalloc_unlocked(size);
  irq_restore(flags);

  return ptr;
}

static void coalesce(void) {
  Block *block = heap_head;

  while (block != NULL && block->next != NULL) {
    if (block->free && block->next->free) {
      block->size += sizeof(Block) + block->next->size;
      block->next = block->next->next;
    } else {
      block = block->next;
    }
  }
}

static void kfree_unlocked(void *ptr) {
  if (ptr == NULL) {
    return;
  }

  uintptr_t addr = (uintptr_t)ptr;
  if ((addr & 15) || addr < (uintptr_t)heap_head + sizeof(Block) || addr >= heap_end) {
    panic("KHeap: pointer is outside of the heap.");
  }

  Block *block = (Block *)ptr - 1;

  if (block->magic != BLOCK_MAGIC) {
    panic("KHeap: bad pointer or corrupted header.");
  }

  if (block->free) {
    panic("KHeap: double free.");
  }

  block->free = 1;
  coalesce();
}

void kfree(void *ptr) {
  uint64_t flags = irq_save();
  kfree_unlocked(ptr);
  irq_restore(flags);
}
