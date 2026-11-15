#include "uart.h"

#include "irq.h"
#include "sched.h"

#include <stdint.h>

#define UART_BASE    0x09000000UL
#define UART_DR      (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_FR      (*(volatile uint32_t *)(UART_BASE + 0x18))
#define UART_FR_TXFF (1 << 5)

#define UART_IMSC (*(volatile uint32_t *)(UART_BASE + 0x38))
#define UART_ICR  (*(volatile uint32_t *)(UART_BASE + 0x44))
#define UART_FR_RXFE (1 << 4)
#define UART_INT_RX  (1 << 4)
#define UART_INT_RT  (1 << 6)

#define RX_BUF_SIZE 256

static volatile char rx_buf[RX_BUF_SIZE];
static volatile uint32_t rx_head;
static volatile uint32_t rx_tail;

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

void uart_rx_init(void) {
  UART_IMSC = UART_INT_RX | UART_INT_RT;
}

void uart_rx_irq(void) {
  while (!(UART_FR & UART_FR_RXFE)) {
    char c = (char)(UART_DR & 0xFF);
    uint32_t next = (rx_head + 1) % RX_BUF_SIZE;

    if (next != rx_tail) {
      rx_buf[rx_head] = c;
      rx_head = next;
    }
  }

  UART_ICR = UART_INT_RX | UART_INT_RT;

  sched_wake((const void *)&rx_head);
}

int uart_getc_nb(void) {
  uint64_t flags = irq_save();
  int c = -1;

  if (rx_tail != rx_head) {
    c = (unsigned char)rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
  }

  irq_restore(flags);

  return c;
}

int uart_getc(void) {
  uint64_t flags = irq_save();

  while (rx_tail == rx_head) {
    sched_block((const void *)&rx_head);
  }

  char c = rx_buf[rx_tail];
  rx_tail = (rx_tail + 1) % RX_BUF_SIZE;

  irq_restore(flags);
  return (unsigned char)c;
}
