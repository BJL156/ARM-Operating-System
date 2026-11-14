#include "elf.h"

#include "kprintf.h"
#include "mem.h"
#include "mmu.h"
#include "pmm.h"

#define PAGE 4096UL

#define FAIL(code) do { \
  kprintf("ELF: fail %d (seg vaddr 0x%lx).\n", (code), ph->p_vaddr); \
  return -1; \
} while (0)

static int range_ok(uint64_t off, uint64_t len, uint64_t size) {
  return off <= size && len <= size - off;
}

static int header_ok(const struct elf64_ehdr *eh, size_t size) {
  if (size < sizeof(*eh)) {
    return 0;
  }

  if (eh->e_ident[0] != 0x7f || eh->e_ident[1] != 'E' ||
      eh->e_ident[2] != 'L'  || eh->e_ident[3] != 'F') {
    return 0;
  }

  if (eh->e_ident[4] != 2 || eh->e_ident[5] != 1) {
    return 0;
  }

  if (eh->e_type != ET_EXEC || eh->e_machine != EM_AARCH64) {
    return 0;
  }

  if (eh->e_phentsize != sizeof(struct elf64_phdr)) {
    return 0;
  }

  return range_ok(eh->e_phoff, (uint64_t)eh->e_phnum * eh->e_phentsize, size);
}

static uint64_t flags_for(uint32_t pf) {
  if (pf & PF_X) {
    return MAP_URX;
  }
  if (pf & PF_W) {
    return MAP_URW;
  }
  return MAP_URO;
}

static int load_segment(ASpace *aspace, const uint8_t *image, size_t size, const struct elf64_phdr *ph) {
  if (ph->p_vaddr & (PAGE - 1)) {
    FAIL(1);
  }

  if (ph->p_vaddr < USER_VA_BASE || ph->p_vaddr > USER_VA_LIMIT) {
    FAIL(2);
  }

  if (ph->p_memsz > USER_VA_LIMIT - ph->p_vaddr) {
    FAIL(3);
  }

  if (ph->p_filesz > ph->p_memsz) {
    FAIL(4);
  }

  if (!range_ok(ph->p_offset, ph->p_filesz, size)) {
    FAIL(5);
  }

  if ((ph->p_flags & PF_W) && (ph->p_flags & PF_X)) {
    FAIL(6);
  }

  uint64_t flags = flags_for(ph->p_flags);

  for (uint64_t off = 0; off < ph->p_memsz; off += PAGE) {
    uint8_t *page = pmm_alloc_zeroed();
    if (page == NULL) {
      FAIL(7);
    }

    if (off < ph->p_filesz) {
      uint64_t n = ph->p_filesz - off;
      if (n > PAGE) {
        n = PAGE;
      }
      memcpy(page, image + ph->p_offset + off, n);
    }

    aspace_map(aspace, ph->p_vaddr + off, (uint64_t)page, flags);
  }
  return 0;
}

int elf_load(ASpace *aspace, const void *image, size_t size, uint64_t *entry) {
  const uint8_t *base = image;
  const struct elf64_ehdr *eh = image;

  if (!header_ok(eh, size)) {
    kprintf("ELF: bad header (size %lu).\n", (uint64_t)size);
    return -1;
  }

  for (int i = 0; i < eh->e_phnum; i++) {
    const struct elf64_phdr *ph = (const struct elf64_phdr *)(base + eh->e_phoff + (uint64_t)i * sizeof(*ph));
    if (ph->p_type != PT_LOAD || ph->p_memsz == 0) {
      continue;
    }

    if (load_segment(aspace, base, size, ph) != 0) {
      return -1;
    }
  }

  if (eh->e_entry < USER_VA_BASE || eh->e_entry >= USER_VA_LIMIT) {
    return -1;
  }
  *entry = eh->e_entry;

  return 0;
}
