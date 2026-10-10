#include "uart.h"

#include <stdint.h>

#define UART_BASE    0x09000000UL
#define UART_DR      (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_FR      (*(volatile uint32_t *)(UART_BASE + 0x18))
#define UART_FR_TXFF (1 << 5)

void uart_putc(char c) {
  while (UART_FR & UART_FR_TXFF) {
  }

  UART_DR = (uint32_t)c;
}

void uart_puts(const char *s) {
  for (int i = 0; s[i] != '\0'; i++) {
    uart_putc(s[i]);
  }
}

void uart_put_hex(uint64_t value, int upper) {
  const char *hex_lookup = upper ? "0123456789ABCDEF" : "0123456789abcdef";

  for (int i = 60; i >= 0; i -= 4) {
    uart_putc(hex_lookup[(value >> i) & 0x0F]);
  }
}

void uart_put_hex_trim(uint64_t value, int upper) {
  const char *hex_lookup = upper ? "0123456789ABCDEF" : "0123456789abcdef";

  int started = 0;

  for (int i = 60; i >= 0; i -= 4) {
    uint8_t digit = (value >> i) & 0x0F;

    if (digit != 0 || started || i == 0) {
      uart_putc(hex_lookup[digit]);
      started = 1;
    }
  }
}

void uart_put_dec(uint64_t value) {
  char buf[20];
  int i = 0;

  if (value == 0) {
    uart_putc('0');
    return;
  }

  while (value > 0) {
    buf[i++] = '0' + (value % 10);
    value /= 10;
  }

  while (i > 0) {
    uart_putc(buf[--i]);
  }
}
