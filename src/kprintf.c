#include "kprintf.h"

#include "uart.h"

void kprintf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  for (int i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] != '%') {
      uart_putc(fmt[i]);
      continue;
    }

    i++;
    if (fmt[i] == '\0') {
      break;
    }

    int is_long = 0;
    if (fmt[i] == 'l') {
      is_long = 1;
      i++;

      if (fmt[i] == '\0') {
        break;
      }
    }

    switch (fmt[i]) {
      case '%': {
        uart_putc('%');
        break;
      }
      case 'c': {
        uart_putc((char)va_arg(ap, int));
        break;
      }
      case 's': {
        uart_puts(va_arg(ap, const char *));
        break;
      }
      case 'u': {
        uint64_t v = is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
        uart_put_dec(v);
        break;
      }
      case 'x': {
        uint64_t v = is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
        uart_put_hex_trim(v, 0);
        break;
      }
      case 'X': {
        uint64_t v = is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
        uart_put_hex_trim(v, 1);
        break;
      }
      case 'd': {
        int64_t v = is_long ? va_arg(ap, long) : va_arg(ap, int);
        if (v < 0) {
          uart_putc('-');
          v = -v;
        }

        uart_put_dec(v);
        break;
      }
      case 'p': {
        uintptr_t v = (uintptr_t)va_arg(ap, void *);
        uart_puts("0x");
        uart_put_hex_trim(v, 0);
        break;
      }
      default: {
        uart_putc('?');
        break;
      }
    }
  }

  va_end(ap);
}
