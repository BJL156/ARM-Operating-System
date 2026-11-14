#ifndef ELF_H
#define ELF_H

#include "aspace.h"

#include <stddef.h>
#include <stdint.h>

#define ET_EXEC    2
#define EM_AARCH64 183
#define PT_LOAD    1
#define PF_X       1
#define PF_W       2
#define PF_R       4

#define USER_VA_LIMIT (USER_VA_BASE + 0x40000000UL)

struct elf64_ehdr {
  uint8_t  e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct elf64_phdr {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

_Static_assert(sizeof(struct elf64_ehdr) == 64, "ehdr size");
_Static_assert(sizeof(struct elf64_phdr) == 56, "phdr size");

int elf_load(ASpace *aspace, const void *image, size_t size, uint64_t *entry);

#endif
