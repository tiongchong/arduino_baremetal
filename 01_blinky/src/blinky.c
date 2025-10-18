#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set pin 5 of PORTB (digital pin 13 on Arduino Uno) as output
    DDRB |= (1 << DDB5);

    while (1) {
        // Toggle LED
        PORTB ^= (1 << PORTB5);
        _delay_ms(100);
    }

    return 0;
}

