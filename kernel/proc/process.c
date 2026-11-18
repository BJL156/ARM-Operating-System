#include "process.h"

#include "aspace.h"
#include "elf.h"
#include "kprintf.h"
#include "mmu.h"
#include "sched.h"
#include "pmm.h"
#include "user.h"

#define USER_STACK_TOP   USER_VA_LIMIT
#define USER_STACK_PAGES 4

Task *process_create(const void *image, size_t size, const char *name) {
  ASpace *aspace = aspace_create();
  uint64_t entry;

  if (elf_load(aspace, image, size, &entry) != 0) {
    kprintf("Process: bad ELF for %s.\n", name);
    return NULL;
  }

  for (uint64_t i = 1; i <= USER_STACK_PAGES; i++) {
    void *page = pmm_alloc_zeroed();
    if (page == NULL) {
      return NULL;
    }
    aspace_map(aspace, USER_STACK_TOP - i * 4096, (uint64_t)page, MAP_URW);
  }

  Task *task = task_create(user_launch, NULL, name);
  if (task == NULL) {
    return NULL;
  }
  task->parent_id = sched_current()->id;

  task->aspace = aspace;
  task->user_entry = entry;
  task->user_sp = USER_STACK_TOP;

  return task;
}
