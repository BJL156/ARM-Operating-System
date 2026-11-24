#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

#include "fs_format.h"

int fs_init(const void *img, size_t size);
uint32_t fs_count(void);
const FSEntry *fs_entry_at(uint32_t idx);
const FSEntry *fs_entry_lookup(const char *name);
const uint8_t *fs_data(const FSEntry *entry);

#endif
