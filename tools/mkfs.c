#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs_format.h"

#define ALIGN16(x) (((x) + 15UL) & ~15UL)

typedef struct {
  const char *name;
  unsigned char *data;
  uint32_t size;
} Input;

static void die(const char *msg, const char *arg) {
  fprintf(stderr, "MKFS: %s: %s.", msg, arg);
  exit(1);
}

static const char *base_name(const char *path) {
  const char *slash = strchr(path, '/');
  return slash ? slash + 1 : path;
}

static unsigned char *read_file(const char *path, uint32_t *size) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    die("Can't open file", path);
  }

  fseek(file, 0, SEEK_END);
  long n = ftell(file);
  fseek(file, 0, SEEK_SET);
  if (n < 0 || n > 16L * 1024 * 1024) {
    die("File unreadable or larger than 16 MiB", path);
  }

  unsigned char *buf = malloc(n > 0 ? (size_t)n : 1);
  if (buf == NULL || fread(buf, 1, (size_t)n, file) != (size_t)n) {
    die("Read failed", path);
  }
  
  fclose(file);

  *size = (uint32_t)n;

  return buf;
}

static void pad_to(FILE *out, uint32_t *pos, uint32_t target) {
  while (*pos < target) {
    fputc(0, out);
    (*pos)++;
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s out.img file\n", argv[0]);
    return 1;
  }

  uint32_t count = (uint32_t)(argc - 2);
  Input *in = calloc(count, sizeof(*in));
  FSEntry *dir = calloc(count, sizeof(*dir));
  if (in == NULL || dir == NULL) {
    die("Out of memory", "");
  }

  uint32_t dir_end = (uint32_t)(sizeof(FSHeader) + count * sizeof(FSEntry));
  uint32_t offset = (uint32_t)ALIGN16(dir_end);

  for (uint32_t i = 0; i < count; i++) {
    const char *path = argv[i + 2];
    in[i].name = base_name(path);

    size_t len = strlen(in[i].name);
    if (len == 0 || len >= FS_NAME_MAX) {
      die("Bad file name length", in[i].name);
    }

    for (uint32_t j = 0; j < i; j++) {
      if (strcmp(in[j].name, in[i].name) == 0) {
        die("Duplicate file name", in[i].name);
      }
    }

    in[i].data = read_file(path, &in[i].size);

    memcpy(dir[i].name, in[i].name, len);
    dir[i].offset = offset;
    dir[i].size = in[i].size;
    offset = (uint32_t)ALIGN16(offset + in[i].size);
  }

  FSHeader hdr;
  memset(&hdr, 0, sizeof(hdr));
  memcpy(hdr.magic, FS_MAGIC, 8);
  hdr.count = count;

  FILE *out = fopen(argv[1], "wb");
  if (out == NULL) {
    die("Can't create", argv[1]);
  }

  fwrite(&hdr, sizeof(hdr), 1, out);
  fwrite(dir, sizeof(*dir), count, out);

  uint32_t pos = dir_end;
  for (uint32_t i = 0; i < count; i++) {
    pad_to(out, &pos, dir[i].offset);
    fwrite(in[i].data, 1, in[i].size, out);
    pos += in[i].size;
  }

  pad_to(out, &pos, offset);
  fclose(out);

  printf("MKFS: Wrote %s (%u files, %u bytes)\n", argv[1], count, offset);

  return 0;
}
