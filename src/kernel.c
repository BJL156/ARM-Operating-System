#include "gic.h"
#include "kprintf.h"
#include "mem.h"
#include "pmm.h"
#include "timer.h"

#include <stddef.h>
#include <stdint.h>

#define RAM_BASE 0x40000000UL
#define RAM_SIZE (128UL * 1024 * 1024)
#define RAM_END  (RAM_BASE + RAM_SIZE)

#define PAGE_SIZE 4096UL

static void check(const char *name, int ok) {
  kprintf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
}

static int bytes_equal(const unsigned char *a, const unsigned char *b, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

static void *alloc_zeroed(void) {
  void *page = pmm_alloc();
  if (page == NULL) {
    return NULL;
  }

  memset(page, 0, PAGE_SIZE);
  return page;
}

static void test_mem(void) {
  unsigned char buf[8];
  for (int i = 0; i < 8; i++) buf[i] = 0;

  unsigned char all_ab[8] = {0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB};
  void *r = memset(buf, 0xAB, 8);
  check("memset fills every byte", bytes_equal(buf, all_ab, 8));
  check("memset returns dst", r == buf);

  for (int i = 0; i < 8; i++) buf[i] = 0;
  unsigned char partial[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0};
  memset(buf, 0xFF, 4);
  check("memset stops at n (no overrun)", bytes_equal(buf, partial, 8));

  for (int i = 0; i < 8; i++) buf[i] = 7;
  memset(buf, 0, 0);
  unsigned char sevens[8] = {7, 7, 7, 7, 7, 7, 7, 7};
  check("memset with n=0 changes nothing", bytes_equal(buf, sevens, 8));

  unsigned char src[16], dst[16];
  for (int i = 0; i < 16; i++) {
    src[i] = (unsigned char)i;
    dst[i] = 0xEE;
  }
  r = memcpy(dst, src, 16);
  check("memcpy copies all bytes", bytes_equal(dst, src, 16));
  check("memcpy returns dst", r == dst);

  for (int i = 0; i < 16; i++) dst[i] = 0xEE;
  memcpy(dst, src, 5);
  unsigned char expect_cpy[16] = {0, 1, 2, 3, 4, 0xEE, 0xEE, 0xEE,
                                  0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE};
  check("memcpy stops at n", bytes_equal(dst, expect_cpy, 16));

  unsigned char a[4] = {1, 2, 3, 4};
  unsigned char b[4] = {1, 2, 3, 4};
  check("memcmp equal -> 0", memcmp(a, b, 4) == 0);

  b[3] = 5;
  check("memcmp a<b -> negative", memcmp(a, b, 4) < 0);
  check("memcmp a>b -> positive", memcmp(b, a, 4) > 0);
  check("memcmp ignores bytes past n", memcmp(a, b, 3) == 0);
  check("memcmp n=0 -> 0", memcmp(a, b, 0) == 0);

  unsigned char hi[1] = {0x80};
  unsigned char lo[1] = {0x01};
  check("memcmp compares as unsigned", memcmp(hi, lo, 1) > 0);

  unsigned char m[10];
  for (int i = 0; i < 10; i++) m[i] = (unsigned char)i;
  memmove(m + 2, m, 6);
  unsigned char expect_fwd[10] = {0, 1, 0, 1, 2, 3, 4, 5, 8, 9};
  check("memmove overlap, dst > src", bytes_equal(m, expect_fwd, 10));

  for (int i = 0; i < 10; i++) m[i] = (unsigned char)i;
  memmove(m, m + 2, 6);
  unsigned char expect_bwd[10] = {2, 3, 4, 5, 6, 7, 6, 7, 8, 9};
  check("memmove overlap, dst < src", bytes_equal(m, expect_bwd, 10));

  for (int i = 0; i < 16; i++) dst[i] = 0;
  memmove(dst, src, 16);
  check("memmove no overlap", bytes_equal(dst, src, 16));
}

static void test_pmm_zeroed(void) {
  unsigned char *p = pmm_alloc();
  memset(p, 0xFF, 4096);
  pmm_free(p);

  unsigned char *q = alloc_zeroed();
  int zero = 1;
  for (int i = 0; i < 4096; i++) {
    if (q[i] != 0) {
      zero = 0;
      break;
    }
  }
  check("pmm_alloc_zeroed returns the same page", q == p);
  check("pmm_alloc_zeroed page is all zero", zero);
  pmm_free(q);
}

extern char __kernel_end[];
extern char _start[];

void kernel_main(void) {
  uint64_t kernel_start = (uint64_t)_start;
  uint64_t kernel_end = (uint64_t)__kernel_end;
  uint64_t ram_end = RAM_END;

  uint64_t free_pages = (ram_end - kernel_end) / PAGE_SIZE;

  kprintf("Hello from ARM Operating System.\n");

  kprintf("Kernel start: %lx\n", (unsigned long)kernel_start);
  kprintf("Kernel end:   %lx\n", (unsigned long)kernel_end);
  kprintf("RAM end:      %lx\n", (unsigned long)ram_end);
  kprintf("Free pages:   %lu\n", (unsigned long)free_pages);

  pmm_init(kernel_end, ram_end);
  kprintf("PMM free pages: %lu\n", pmm_free_count());
  kprintf("PMM allocate 1: %p\n", pmm_alloc());
  uint64_t *ptr = pmm_alloc();
  kprintf("PMM allocate 2: %p\n", ptr);
  kprintf("PMM allocate 3: %p\n", pmm_alloc());

  pmm_free(ptr);

  kprintf("PMM allocate 4: %p\n", pmm_alloc());

  uint64_t count = 0;
  while (pmm_alloc() != NULL) {
    count++;
  }
  kprintf("Allocated %lu pages, free now %lu\n", count, pmm_free_count());
  
  test_mem();
  test_pmm_zeroed();

  gic_init();
  gic_enable_irq(30);
  timer_start(timer_freq());
  asm volatile("msr daifclr, #2");

  while (1) {
    asm volatile("wfi");
  }
}
