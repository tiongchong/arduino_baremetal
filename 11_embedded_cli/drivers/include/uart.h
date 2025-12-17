#ifndef UART_H
#define UART_H

#include "avr/io.h"
#include "config.h"

#include <stdbool.h>
#include <avr/pgmspace.h>

#define BAUD 57600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void);

void uart_putc(char c);
void uart_puts(const char *str);
void uart_print_ulong_width(unsigned long n, int width, char pad, bool plus, bool is_signed);
void uart_print_dec(int n);
void uart_print_hex_width(unsigned long n, int width, char pad, bool alt);
void uart_print_oct_width(unsigned long n, int width, char pad, bool alt);
void uart_print_ptr(void *ptr);
void uart_print_float_int(long val, int frac_digits, bool plus);
void uart_print_scientific(double val, int precision, char exp_char, bool plus);

char uart_getc(void);

#endif // UART_H