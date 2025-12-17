#include "avr/io.h"     // Include custom AVR I/O header with register definitions and bit macros

// -----------------------------------------------------------------------------
// delay_ms - Basic software delay function
// -----------------------------------------------------------------------------
// Provides an approximate blocking delay in milliseconds.
// This delay is implemented using nested loops with 'nop' instructions,
// which waste CPU cycles to create the delay.
//
// Parameters:
//   ms - number of milliseconds to delay (approximate)
//
// Note:
// - The actual delay duration depends on CPU clock frequency and compiler optimizations.
// - The volatile keyword on the loop counter prevents the compiler from optimizing out the loop.
// - This is a simple, non-precise delay, suitable for basic timing needs in small demos.
//
// Usage example:
//   delay_ms(500); // Delay for approximately 500 milliseconds
void delay_ms(uint16_t ms) {
    while (ms--) {                          // Outer loop runs once per millisecond
        for (volatile uint16_t i = 0; i < 1000; i++) {
            __asm__ __volatile__("nop");   // Single assembly no-operation instruction to waste a clock cycle
        }
    }
}

// -----------------------------------------------------------------------------
// main - Program entry point
// -----------------------------------------------------------------------------
// This example program toggles the onboard LED connected to PB5 (Arduino digital pin 13)
// at approximately 1 Hz (500 ms ON, 500 ms OFF).
//
// Steps performed:
// 1. Configure PB5 as an output by setting the corresponding bit in the DDRB register.
// 2. Enter an infinite loop where:
//    - The state of PB5 is toggled using the XOR operator.
//    - A delay of 500 ms is inserted between toggles.
//
// Hardware notes:
// - PB5 corresponds to the physical pin connected to the onboard LED on Arduino Uno and compatible boards.
// - Direct register manipulation provides fast and efficient I/O control.
//
int main(void) {
    // Configure PB5 pin as output:
    // DDRB (Data Direction Register for port B) controls pin direction:
    // - Setting a bit to 1 configures the pin as output.
    // - Clearing a bit to 0 configures the pin as input.
    DDRB |= (1 << PB5);   // Set bit 5 of DDRB to 1

    while (1) {
        // Toggle the output state of PB5:
        // XORing the PORTB register bit with 1 flips its state.
        // If the pin was HIGH, it becomes LOW, and vice versa.
        PORTB ^= (1 << PB5);

        // Wait approximately 500 ms to make the toggle visible to human eye
        delay_ms(500);
    }
    return 0;
}
