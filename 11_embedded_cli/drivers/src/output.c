#include "output.h"

// Default output function pointer (initially NULL)
static output_putc_t _current_output_putc = 0;

void output_set_putc(output_putc_t putc_func) {
    _current_output_putc = putc_func;
}

output_putc_t output_get_putc(void) {
    return _current_output_putc;
}

