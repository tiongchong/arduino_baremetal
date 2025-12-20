#include "stdio.h"
#include "output.h"
#include "stdarg.h"
#include "pgmspace.h"
#include "stdbool.h"

// --- Helper functions for printf formatting ---

// Integer printing helper
void stdio_print_ulong_width(output_putc_t putc, unsigned long n, int width, char pad, bool plus, bool is_signed) {
    static const unsigned long powers32[] PROGMEM = {
        1000000000UL,100000000UL,10000000UL,1000000UL,
        100000UL,10000UL,1000UL,100UL,10UL,1UL
    };
    
    bool started = false;
    char count;
    int printed = 0;

    if (plus && is_signed) putc('+');

    for (int i = 0; i < 10; i++) {
        unsigned long power = pgm_read_dword(&powers32[i]);
        count = 0;
        while (n >= power) { n -= power; count++; }
        if (count > 0 || started || i == 9) {
            putc('0' + count);
            started = true;
            printed++;
        } else if (pad == '0') {
            putc('0');
            printed++;
        } else if (width-- > printed) {
            putc(' ');
            printed++;
        }
    }
}

// Hexadecimal printing helper
void stdio_print_hex_width(output_putc_t putc, unsigned long n, int width, char pad, bool alt) {
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
    if (alt) { putc('0'); putc('X'); }
    for (int i = len; i < width; i++) putc(pad);
    for (int i = len - 1; i >= 0; i--) putc(buf[i]);
}

// Octal printing helper
void stdio_print_oct_width(output_putc_t putc, unsigned long n, int width, char pad, bool alt) {
    char buf[12];
    int len = 0;
    if (n == 0) buf[len++] = '0';
    else {
        while (n > 0) { buf[len++] = '0' + (n & 7); n >>= 3; }
    }
    if (alt) putc('0');
    for (int i = len; i < width; i++) putc(pad);
    for (int i = len - 1; i >= 0; i--) putc(buf[i]);
}

// Float printing helper
void stdio_print_float_int(output_putc_t putc, long val, int frac_digits, bool plus) {
    if (val < 0) {
        putc('-');
        val = -val;
    } else if (plus) {
        putc('+');
    }

    long scale = 1;
    for (int i = 0; i < frac_digits; i++) scale *= 10;

    long int_part = val / scale;
    long frac_part = val % scale;

    stdio_print_ulong_width(putc, (unsigned long)int_part, 0, ' ', false, false);
    putc('.');

    // Print fractional part with leading zeros
    long divisor = scale / 10;
    for (int i = 0; i < frac_digits; i++) {
        putc('0' + (frac_part / divisor) % 10);
        divisor /= 10;
    }
}

// Scientific notation printing helper
void stdio_print_scientific(output_putc_t putc, double val, int precision, char exp_char, bool plus) {
    // Handle zero separately
    if (val == 0.0) {
        putc('0');
        putc('.');
        for (int i = 0; i < precision; i++) putc('0');
        putc(exp_char);
        putc('+');
        putc('0');
        putc('0');
        return;
    }

    // Handle sign
    if (val < 0) {
        putc('-');
        val = -val;
    } else if (plus) {
        putc('+');
    }

    int exponent = 0;

    // Normalize val to [1.0, 10.0)
    while (val >= 10.0) { val /= 10.0; exponent++; }
    while (val < 1.0) { val *= 10.0; exponent--; }

    // Convert to integer fixed-point for printing
    long scale = 1;
    for (int i = 0; i < precision; i++) scale *= 10;
    long fixed_val = (long)(val * scale + 0.5); // rounding

    stdio_print_float_int(putc, fixed_val, precision, false);

    // Print exponent part
    putc(exp_char);
    if (exponent >= 0) putc('+');
    else { putc('-'); exponent = -exponent; }

    // Always print exponent with 2 digits
    putc('0' + (exponent / 10) % 10);
    putc('0' + (exponent % 10));
}

// --- Main printf function ---
// Uses output_putc function pointer to redirect output
void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    output_putc_t putc = output_get_putc();
    if (!putc) return; // No output function set

    char c;
    // For sprintf, format string is in RAM (not PROGMEM)
    while ((c = *fmt++)) {
        if (c == '%') {
            bool plus = false;
            bool alt = false;
            char pad = ' ';
            int width = 0;
            bool long_flag = false;
            int precision = 6; // default float precision

            c = *fmt++;
            while (c == '+' || c == '0' || c == '#') {
                if (c == '+') plus = true;
                if (c == '0') pad = '0';
                if (c == '#') alt = true;
                c = *fmt++;
            }

            while (c >= '0' && c <= '9') {
                width = width * 10 + (c - '0');
                c = *fmt++;
            }

            if (c == '.') {
                c = *fmt++;
                precision = 0;
                while (c >= '0' && c <= '9') {
                    precision = precision * 10 + (c - '0');
                    c = *fmt++;
                }
            }

            if (c == 'l') { long_flag = true; c = *fmt++; }

            switch (c) {
                case 'c': putc((char)va_arg(args, int)); break;
                case 's': {
                    const char* str = va_arg(args, const char*);
                    while (*str) putc(*str++);
                    break;
                }
                case 'd': {
                    long val = long_flag ? va_arg(args, long) : va_arg(args, int);
                    if (val < 0) { 
                        putc('-'); 
                        stdio_print_ulong_width(putc, (unsigned long)(-val), width, pad, false, false); 
                    } else { 
                        stdio_print_ulong_width(putc, (unsigned long)val, width, pad, plus, true); 
                    }
                    break;
                }
                case 'u': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    stdio_print_ulong_width(putc, val, width, pad, false, false);
                    break;
                }
                case 'x': case 'X': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    stdio_print_hex_width(putc, val, width, pad, alt);
                    break;
                }
                case 'o': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    stdio_print_oct_width(putc, val, width, pad, alt);
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void*);
                    putc('0');
                    putc('x');
                    stdio_print_hex_width(putc, (unsigned int)ptr, 0, '0', false);
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);
                    long scale = 1;
                    for (int i = 0; i < precision; i++) scale *= 10;
                    stdio_print_float_int(putc, (long)(val * scale + 0.5), precision, plus);
                    break;
                }
                case 'e': case 'E': {
                    double val = va_arg(args, double);
                    stdio_print_scientific(putc, val, precision, c, plus);
                    break;
                }
                case '%': putc('%'); break;
                default: putc(c); break;
            }
        } else {
            if (c == '\n') { putc('\r'); putc('\n'); }
            else putc(c);
        }
    }

    va_end(args);
}

// --- Buffer-based helper functions for sprintf ---

// Buffer context structure
typedef struct {
    char *buf;
    int pos;
    int max_size;
} buffer_ctx_t;

// Buffer putc function
static void buffer_putc(char c, buffer_ctx_t *ctx) {
    if (ctx->pos < ctx->max_size - 1) {
        ctx->buf[ctx->pos++] = c;
    }
}

// Buffer-based integer printing helper
static void sprintf_print_ulong_width(buffer_ctx_t *ctx, unsigned long n, int width, char pad, bool plus, bool is_signed) {
    static const unsigned long powers32[] PROGMEM = {
        1000000000UL,100000000UL,10000000UL,1000000UL,
        100000UL,10000UL,1000UL,100UL,10UL,1UL
    };
    
    bool started = false;
    char count;
    int printed = 0;

    if (plus && is_signed) buffer_putc('+', ctx);

    for (int i = 0; i < 10; i++) {
        unsigned long power = pgm_read_dword(&powers32[i]);
        count = 0;
        while (n >= power) { n -= power; count++; }
        if (count > 0 || started || i == 9) {
            buffer_putc('0' + count, ctx);
            started = true;
            printed++;
        } else if (pad == '0') {
            buffer_putc('0', ctx);
            printed++;
        } else if (width-- > printed) {
            buffer_putc(' ', ctx);
            printed++;
        }
    }
}

// Buffer-based hexadecimal printing helper
static void sprintf_print_hex_width(buffer_ctx_t *ctx, unsigned long n, int width, char pad, bool alt) {
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
    if (alt) { buffer_putc('0', ctx); buffer_putc('X', ctx); }
    for (int i = len; i < width; i++) buffer_putc(pad, ctx);
    for (int i = len - 1; i >= 0; i--) buffer_putc(buf[i], ctx);
}

// Buffer-based octal printing helper
static void sprintf_print_oct_width(buffer_ctx_t *ctx, unsigned long n, int width, char pad, bool alt) {
    char buf[12];
    int len = 0;
    if (n == 0) buf[len++] = '0';
    else {
        while (n > 0) { buf[len++] = '0' + (n & 7); n >>= 3; }
    }
    if (alt) buffer_putc('0', ctx);
    for (int i = len; i < width; i++) buffer_putc(pad, ctx);
    for (int i = len - 1; i >= 0; i--) buffer_putc(buf[i], ctx);
}

// Buffer-based float printing helper
static void sprintf_print_float_int(buffer_ctx_t *ctx, long val, int frac_digits, bool plus) {
    if (val < 0) {
        buffer_putc('-', ctx);
        val = -val;
    } else if (plus) {
        buffer_putc('+', ctx);
    }

    long scale = 1;
    for (int i = 0; i < frac_digits; i++) scale *= 10;

    long int_part = val / scale;
    long frac_part = val % scale;

    sprintf_print_ulong_width(ctx, (unsigned long)int_part, 0, ' ', false, false);
    buffer_putc('.', ctx);

    // Print fractional part with leading zeros
    long divisor = scale / 10;
    for (int i = 0; i < frac_digits; i++) {
        buffer_putc('0' + (frac_part / divisor) % 10, ctx);
        divisor /= 10;
    }
}

// Buffer-based scientific notation printing helper
static void sprintf_print_scientific(buffer_ctx_t *ctx, double val, int precision, char exp_char, bool plus) {
    // Handle zero separately
    if (val == 0.0) {
        buffer_putc('0', ctx);
        buffer_putc('.', ctx);
        for (int i = 0; i < precision; i++) buffer_putc('0', ctx);
        buffer_putc(exp_char, ctx);
        buffer_putc('+', ctx);
        buffer_putc('0', ctx);
        buffer_putc('0', ctx);
        return;
    }

    // Handle sign
    if (val < 0) {
        buffer_putc('-', ctx);
        val = -val;
    } else if (plus) {
        buffer_putc('+', ctx);
    }

    int exponent = 0;

    // Normalize val to [1.0, 10.0)
    while (val >= 10.0) { val /= 10.0; exponent++; }
    while (val < 1.0) { val *= 10.0; exponent--; }

    // Convert to integer fixed-point for printing
    long scale = 1;
    for (int i = 0; i < precision; i++) scale *= 10;
    long fixed_val = (long)(val * scale + 0.5); // rounding

    sprintf_print_float_int(ctx, fixed_val, precision, false);

    // Print exponent part
    buffer_putc(exp_char, ctx);
    if (exponent >= 0) buffer_putc('+', ctx);
    else { buffer_putc('-', ctx); exponent = -exponent; }

    // Always print exponent with 2 digits
    buffer_putc('0' + (exponent / 10) % 10, ctx);
    buffer_putc('0' + (exponent % 10), ctx);
}

// --- sprintf function ---
// Writes formatted string to buffer
// Note: format string is expected to be in RAM, not PROGMEM
int sprintf(char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    buffer_ctx_t ctx;
    ctx.buf = str;
    ctx.pos = 0;
    // Set a large max_size to avoid buffer overflow checks
    // Caller is responsible for ensuring buffer is large enough
    ctx.max_size = 32767; // Maximum reasonable size for embedded systems

    char c;
    // For sprintf, format string is in RAM (not PROGMEM)
    while ((c = *fmt++)) {
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
                case 'c': buffer_putc((char)va_arg(args, int), &ctx); break;
                case 's': {
                    const char* s = va_arg(args, const char*);
                    while (*s) buffer_putc(*s++, &ctx);
                    break;
                }
                case 'd': {
                    long val = long_flag ? va_arg(args, long) : va_arg(args, int);
                    if (val < 0) { 
                        buffer_putc('-', &ctx); 
                        sprintf_print_ulong_width(&ctx, (unsigned long)(-val), width, pad, false, false); 
                    } else { 
                        sprintf_print_ulong_width(&ctx, (unsigned long)val, width, pad, plus, true); 
                    }
                    break;
                }
                case 'u': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    sprintf_print_ulong_width(&ctx, val, width, pad, false, false);
                    break;
                }
                case 'x': case 'X': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    sprintf_print_hex_width(&ctx, val, width, pad, alt);
                    break;
                }
                case 'o': {
                    unsigned long val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                    sprintf_print_oct_width(&ctx, val, width, pad, alt);
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void*);
                    buffer_putc('0', &ctx);
                    buffer_putc('x', &ctx);
                    sprintf_print_hex_width(&ctx, (unsigned int)ptr, 0, '0', false);
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);
                    long scale = 1;
                    for (int i = 0; i < precision; i++) scale *= 10;
                    sprintf_print_float_int(&ctx, (long)(val * scale + 0.5), precision, plus);
                    break;
                }
                case 'e': case 'E': {
                    double val = va_arg(args, double);
                    sprintf_print_scientific(&ctx, val, precision, c, plus);
                    break;
                }
                case '%': buffer_putc('%', &ctx); break;
                default: buffer_putc(c, &ctx); break;
            }
        } else {
            // For sprintf, don't convert \n to \r\n
            buffer_putc(c, &ctx);
        }
    }

    // Null-terminate the string
    buffer_putc('\0', &ctx);

    va_end(args);
    return ctx.pos - 1; // Return number of characters written (excluding null terminator)
}

