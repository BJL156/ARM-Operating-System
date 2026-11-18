#include "process.h"

#include "aspace.h"
#include "elf.h"
#include "kprintf.h"
#include "mmu.h"
#include "sched.h"

#include "mem.h"
#include "pmm.h"
#include "user.h"

#define USER_STACK_TOP   USER_VA_LIMIT
#define USER_STACK_PAGES 4

#define ALIGN16(x)     (((x) + 15UL) & ~15UL)
#define ARGS_MAX_BYTES 2048

static size_t arg_len(const char *s) {
  size_t n = 0;

  while (s[n]) {
    n++;
  }

  return n;
}

Task *process_create(const void *image, size_t size, const char *name, int argc, const char *const *argv) {
  size_t ptr_bytes = ((size_t)argc + 1) * 8;
  size_t str_bytes = 0;
  for (int i = 0; i < argc; i++) {
    str_bytes += arg_len(argv[i]) + 1;
  }

  size_t block = ALIGN16(ptr_bytes + str_bytes);
  if (block > ARGS_MAX_BYTES) {
    return NULL;
  }

  ASpace *aspace = aspace_create();
  uint64_t entry;

  if (elf_load(aspace, image, size, &entry) != 0) {
    kprintf("Process: bad ELF for %s.\n", name);

    aspace_destroy(aspace);
    return NULL;
  }

  uint8_t *top_page = NULL;

  for (uint64_t i = 1; i <= USER_STACK_PAGES; i++) {
    void *page = pmm_alloc_zeroed();
    if (page == NULL) {
      aspace_destroy(aspace);
      return NULL;
    }

    if (i == 1) {
      top_page = page;
    } 

    aspace_map(aspace, USER_STACK_TOP - i * 4096, (uint64_t)page, MAP_URW);
  }

  uint64_t base = USER_STACK_TOP - block;
  uint8_t *mem = top_page + (4096 - block);
  uint64_t *ptrs = (uint64_t *)mem;
  char *s = (char *)mem + ptr_bytes;
  uint64_t s_va = base + ptr_bytes;

  for (int i = 0; i < argc; i++) {
    size_t n = arg_len(argv[i]) + 1;
    memcpy(s, argv[i], n);
    ptrs[i] = s_va;
    s += n;
    s_va += n;
  }

  ptrs[argc] = 0;

  Task *task = task_create(user_launch, NULL, name);
  if (task == NULL) {
    aspace_destroy(aspace);
    return NULL;
  }
  task->parent_id = sched_current()->id;

  task->aspace = aspace;
  task->user_entry = entry;
  task->user_sp = base;
  task->user_argc = (uint64_t)argc;
  task->user_argv = base;

  return task;
}
