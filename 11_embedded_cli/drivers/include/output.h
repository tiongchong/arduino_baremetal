#ifndef OUTPUT_H
#define OUTPUT_H

#include "stdint.h"

// Function pointer type for character output
// This allows printf to be redirected to different output drivers
typedef void (*output_putc_t)(char c);

// Set the output function for printf
// Call this with uart_putc, vga_putc, or any other output function
void output_set_putc(output_putc_t putc_func);

// Get the current output function
output_putc_t output_get_putc(void);

// Default output function (can be set to NULL to disable)
extern output_putc_t _output_putc;

#endif // OUTPUT_H

