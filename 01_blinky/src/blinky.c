#include <avr/io.h>
// #include <util/delay.h>

static void delay_cycles(unsigned long cycles)
{
    while(cycles) {
        __asm__ __volatile__ (
            "nop\n\t"
            "nop\n\t"
            "nop"
        );
        cycles--;
    }
}

#if defined(__clang__)
int app_main(void)
#else
int main(void)
#endif
{
    // Set pin 5 of PORTB (digital pin 13 on Arduino Uno) as output
    DDRB |= (1 << DDB5);

    while (1) {
        // Toggle LED
        PORTB ^= (1 << PORTB5);
        // _delay_ms(100);
        delay_cycles(1600000);
    }

    return 0;
}

