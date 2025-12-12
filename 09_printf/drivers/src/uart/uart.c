#include "avr/io.h"
#include "uart.h"
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdbool.h>

// --- UART init / putchar ---
void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);
    UCSR0B = (1 << TXEN0); // Enable TX only
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_puts(const char *str) {
    char c;
    while ((c = pgm_read_byte(str++))) {
        uart_putc(c);
    }
}

// --- Integer printing helpers ---
static const unsigned long powers32[] PROGMEM = {
    1000000000UL,100000000UL,10000000UL,1000000UL,
    100000UL,10000UL,1000UL,100UL,10UL,1UL
};

void uart_print_ulong_width(unsigned long n, int width, char pad, bool plus, bool is_signed) {
    bool started = false;
    char count;
    int printed = 0;

    if (plus && is_signed) uart_putc('+');

    for (int i = 0; i < 10; i++) {
        unsigned long power = pgm_read_dword(&powers32[i]);
        count = 0;
        while (n >= power) { n -= power; count++; }
        if (count > 0 || started || i == 9) {
            uart_putc('0' + count);
            started = true;
            printed++;
        } else if (pad == '0') {
            uart_putc('0');
            printed++;
        } else if (width-- > printed) {
            uart_putc(' ');
            printed++;
        }
    }
}

void uart_print_dec(int n) {
    if (n < 0) {
        uart_putc('-');
        uart_print_ulong_width((unsigned long)(-((long)n)), 0, ' ', false, false);
    } else {
        uart_print_ulong_width((unsigned long)n, 0, ' ', false, true);
    }
}

void uart_print_hex_width(unsigned long n, int width, char pad, bool alt) {
    char buf[8];
    int len = 0;
    if (n == 0) buf[len++] = '0';
    else {
        while (n > 0) {
            unsigned int digit = n & 0xF;
            buf[len++] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
            n >>= 4;
        }
    }
    if (alt) { uart_putc('0'); uart_putc('X'); }
    for (int i = len; i < width; i++) uart_putc(pad);
    for (int i = len - 1; i >= 0; i--) uart_putc(buf[i]);
}

void uart_print_oct_width(unsigned long n, int width, char pad, bool alt) {
    char buf[12];
    int len = 0;
    if (n == 0) buf[len++] = '0';
    else {
        while (n > 0) { buf[len++] = '0' + (n & 7); n >>= 3; }
    }
    if (alt) uart_putc('0');
    for (int i = len; i < width; i++) uart_putc(pad);
    for (int i = len - 1; i >= 0; i--) uart_putc(buf[i]);
}

void uart_print_ptr(void *ptr) {
    unsigned int addr = (unsigned int)ptr;
    uart_puts("0x");
    uart_print_hex_width(addr, 0, '0', false);
}

// Print a "float" as integer-based fixed-point
void uart_print_float_int(long val, int frac_digits, bool plus) {
    if (val < 0) {
        uart_putc('-');
        val = -val;
    } else if (plus) {
        uart_putc('+');
    }

    long scale = 1;
    for (int i = 0; i < frac_digits; i++) scale *= 10;

    long int_part = val / scale;
    long frac_part = val % scale;

    uart_print_ulong_width((unsigned long)int_part, 0, ' ', false, false);
    uart_putc('.');

    // Print fractional part with leading zeros
    long divisor = scale / 10;
    for (int i = 0; i < frac_digits; i++) {
        uart_putc('0' + (frac_part / divisor) % 10);
        divisor /= 10;
    }
}

void uart_print_scientific(double val, int precision, char exp_char, bool plus) {
    // Handle zero separately
    if (val == 0.0) {
        uart_putc('0');
        uart_putc('.');
        for (int i = 0; i < precision; i++) uart_putc('0');
        uart_putc(exp_char);
        uart_putc('+');
        uart_putc('0');
        uart_putc('0');
        return;
    }

    // Handle sign
    if (val < 0) {
        uart_putc('-');
        val = -val;
    } else if (plus) {
        uart_putc('+');
    }

    int exponent = 0;

    // Normalize val to [1.0, 10.0)
    while (val >= 10.0) { val /= 10.0; exponent++; }
    while (val < 1.0) { val *= 10.0; exponent--; }

    // Convert to integer fixed-point for printing
    long scale = 1;
    for (int i = 0; i < precision; i++) scale *= 10;
    long fixed_val = (long)(val * scale + 0.5); // rounding

    uart_print_float_int(fixed_val, precision, false);

    // Print exponent part
    uart_putc(exp_char);
    if (exponent >= 0) uart_putc('+');
    else { uart_putc('-'); exponent = -exponent; }

    // Always print exponent with 2 digits
    uart_putc('0' + (exponent / 10) % 10);
    uart_putc('0' + (exponent % 10));
}

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