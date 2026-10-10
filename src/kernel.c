#include "kprintf.h"
#include "uart.h"

extern char _start[];

void kernel_main(void) {
  uart_puts("Hello from ARM Operating System.\n");

  uart_puts("0x");
  uart_put_hex(0xDEADBEEF, 0);
  uart_putc('\n');

  uart_put_dec(0);
  uart_putc('\n');
  uart_put_dec(7);
  uart_putc('\n');
  uart_put_dec(12345);
  uart_putc('\n');
  uart_put_dec(18446744073709551615ULL);
  uart_putc('\n');

  kprintf("Hello from %s\n", "ARM Operating System");
  kprintf("char: %c, percent: 100%%\n", 'A');
  kprintf("unsigned: %u\n", 12345);
  kprintf("0x%x 0x%X\n", 0xBEEF, 0xBEEF);
  kprintf("%d %d %d\n", 0, 42, -42);
  kprintf("%d\n", -2147483647 - 1);
  kprintf("100%");
  kprintf("\nstill alive\n");

  kprintf("%lu\n", 18446744073709551615UL);
  kprintf("%lx\n", 0xDEADBEEFCAFEUL);
  kprintf("%ld\n", -5000000000L);
  kprintf("%p\n", (void *)0x40080000);
  kprintf("%x\n", 0xDEADBEEFCAFEUL);

  kprintf("kernel at: %p\n", (void *)_start);

  while (1) {  
  }
}
