#include "fs.h"

#include "mem.h"

static const uint8_t *image;
static size_t image_size;
static uint32_t file_count;

int fs_init(const void *img, size_t size) {
  const FSHeader *hdr = img;

  if (size < sizeof(*hdr) || memcmp(hdr->magic, FS_MAGIC, 8) != 0) {
    return -1;
  }

  uint64_t max_entries = (size - sizeof(*hdr)) / sizeof(FSEntry);
  if (hdr->count > max_entries) {
    return -1;
  }

  uint64_t dir_end = sizeof(*hdr) + (uint64_t)hdr->count * sizeof(FSEntry);

  const FSEntry *dir = (const FSEntry *)((const uint8_t *)img + sizeof(*hdr));

  for (uint32_t i = 0; i < hdr->count; i++) {
    const FSEntry *entry = &dir[i];

    if (entry->name[0] == '\0' || entry->name[FS_NAME_MAX - 1] != '\0') {
      return -1;
    }

    if (entry->offset < dir_end || entry->offset > size || entry->size > size - entry->offset) {
      return -1;
    }
  }

  image = img;
  image_size = size;
  file_count = hdr->count;

  return 0;
}

uint32_t fs_count(void) {
  return file_count;
}

const FSEntry *fs_entry_at(uint32_t idx) {
  if (idx >= file_count) {
    return NULL;
  }

  return (const FSEntry *)(image + sizeof(FSHeader)) + idx;
}

const FSEntry *fs_entry_lookup(const char *name) {
  for (uint32_t i = 0; i < file_count; i++) {
    const FSEntry *entry = fs_entry_at(i);
    if (strcmp(name, entry->name) == 0) {
      return entry;
    }
  }

  return NULL;
}

const uint8_t *fs_data(const FSEntry *entry) {
  return image + entry->offset;
}
