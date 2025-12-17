#include "uart.h"
#include <stdarg.h>

// --- Main printf function ---
void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char c;
    while ((c = pgm_read_byte(fmt++))) {
        if (c == '%') {
            bool plus = false;
            bool alt = false;
            char pad = ' ';
            int width = 0;
            bool long_flag = false;
            int precision = 6; // default float precision

            c = pgm_read_byte(fmt++);
            while (c == '+' || c == '0' || c == '#') {
                if (c == '+') plus = true;
                if (c == '0') pad = '0';
                if (c == '#') alt = true;
                c = pgm_read_byte(fmt++);
            }

            while (c >= '0' && c <= '9') {
                width = width * 10 + (c - '0');
                c = pgm_read_byte(fmt++);
            }

            if (c == '.') {
                c = pgm_read_byte(fmt++);
                precision = 0;
                while (c >= '0' && c <= '9') {
                    precision = precision * 10 + (c - '0');
                    c = pgm_read_byte(fmt++);
                }
            }

            if (c == 'l') { long_flag = true; c = pgm_read_byte(fmt++); }

            switch (c) {
                case 'c': uart_putc((char)va_arg(args, int)); break;
                case 's': uart_puts(va_arg(args, const char*)); break;
                case 'd': {
                    long val = long_flag ? va_arg(args, long) : va_arg(args, int);
                    if (val < 0) { uart_putc('-'); uart_print_ulong_width((unsigned long)(-val), width, pad, false, false); }
                    else { uart_print_ulong_width((unsigned long)val, width, pad, plus, true); }
                    break;
                }
                case 'u': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    uart_print_ulong_width(val, width, pad, false, false);
                    break;
                }
                case 'x': case 'X': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    uart_print_hex_width(val, width, pad, alt);
                    break;
                }
                case 'o': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    uart_print_oct_width(val, width, pad, alt);
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void*);
                    uart_print_ptr(ptr);
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);
                    long scale = 1;
                    for (int i = 0; i < precision; i++) scale *= 10;
                    uart_print_float_int((long)(val * scale + 0.5), precision, plus); // proper rounding
                    break;
                }
                case 'e': case 'E': {
                    double val = va_arg(args, double);
                    uart_print_scientific(val, precision, c, plus);
                    break;
                }
                case '%': uart_putc('%'); break;
                default: uart_putc(c); break;
            }
        } else {
            if (c == '\n') { uart_putc('\r'); uart_putc('\n'); }
            else uart_putc(c);
        }
    }

    va_end(args);
}
