#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_IRQ 33

void uart_putc(char c);
void uart_puts(const char *s);

void uart_put_hex(uint64_t value, int upper);
void uart_put_hex_trim(uint64_t value, int upper);
void uart_put_dec(uint64_t value);

void uart_rx_init(void);
void uart_rx_irq(void);
int uart_getc_nb(void);
int uart_getc(void);

#endif
