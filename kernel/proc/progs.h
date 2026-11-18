#ifndef PROGS_H
#define PROGS_H

typedef struct {
  const char *name;
  const char *start;
  const char *end;
} Prog;

const Prog *prog_find(const char *name);

#endif
