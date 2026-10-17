#include "pmm.h"

#include "panic.h"

#include <stddef.h>

#define PAGE_SIZE 4096

typedef struct FreePage {
  struct FreePage *next;
} FreePage;

static FreePage *head = 0;
static int free_page_count = 0;

void pmm_init(uintptr_t start, uintptr_t end) {
  head = 0;
  free_page_count = 0;

  if (start % PAGE_SIZE != 0 || end % PAGE_SIZE != 0) {
    return;
  }

  for (uintptr_t addr = start; addr + PAGE_SIZE <= end; addr += PAGE_SIZE) {
    FreePage *page = (FreePage *)addr;
    page->next = head;
    head = page;
    free_page_count++;
  }
}

void *pmm_alloc(void) {
  if (head == NULL) {
    return NULL;
  }

  FreePage *page = head;
  head = head->next;
  free_page_count--;

  return (void *)page;
}

void pmm_free(void *ptr) {
  if (ptr == NULL) {
    return;
  }

  if ((uintptr_t)ptr & (PAGE_SIZE - 1)) {
    panic("Misaligned page pointer.");
  }

  FreePage *page = (FreePage *)ptr;
  page->next = head;
  head = page;
  free_page_count++;
}

uint64_t pmm_free_count(void) {
  return free_page_count;
}
