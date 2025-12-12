#include "avr/io.h"
#include "uart.h"

// --- Main program ---
int main(void) {
    uart_init();

    // --- Test Cases ---
    printf("Char: %c, String: %s\n", 'A', "Hello World");
    printf("Int: %+05d, UInt: %u\n", -123, 456);
    long big_num = 1234567890L;
    printf("Long: %ld\n", big_num);
    int val = 255;
    printf("Hex: %#x, Octal: %#o, Pointer: %p\n", val, val, &val);

    double pi = 3.14159265;
    double small = 0.0001234;
    printf("Float: %.4f, Sci: %.3e\n", pi, small);

    while (1) { }
    return 0;
}
