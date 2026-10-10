#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_putc(char c);
void uart_puts(const char *s);

void uart_put_hex(uint64_t value, int upper);
void uart_put_hex_trim(uint64_t value, int upper);
void uart_put_dec(uint64_t value);

#endif
