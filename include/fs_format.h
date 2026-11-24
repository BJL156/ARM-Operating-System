#ifndef FS_FORMAT_H
#define FS_FORMAT_H

#include <stdint.h>

#define FS_MAGIC    "BRAYFS01"
#define FS_NAME_MAX 48

typedef struct {
  char magic[8];
  uint32_t count;
  uint32_t reserved;
} FSHeader;

typedef struct {
  char name[FS_NAME_MAX];
  uint32_t offset;
  uint32_t size;
  uint32_t flags;
  uint32_t reserved;
} FSEntry;

_Static_assert(sizeof(FSHeader) == 16, "FSHeader size");
_Static_assert(sizeof(FSEntry) == 64, "FSEntry size");

#endif
