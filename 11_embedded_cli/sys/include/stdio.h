#ifndef STDIO_H
#define STDIO_H

#include "output.h"
#include "stdint.h"
#include "stddef.h"

// Forward declarations for printf helper functions
// These take the output function pointer as first parameter
void stdio_print_ulong_width(output_putc_t putc, unsigned long n, int width, char pad, bool plus, bool is_signed);
void stdio_print_hex_width(output_putc_t putc, unsigned long n, int width, char pad, bool alt);
void stdio_print_oct_width(output_putc_t putc, unsigned long n, int width, char pad, bool alt);
void stdio_print_float_int(output_putc_t putc, long val, int frac_digits, bool plus);
void stdio_print_scientific(output_putc_t putc, double val, int precision, char exp_char, bool plus);

// Main printf function
void printf(const char *fmt, ...);

// sprintf function - writes formatted string to buffer
int sprintf(char *str, const char *fmt, ...);

#endif // STDIO_H

