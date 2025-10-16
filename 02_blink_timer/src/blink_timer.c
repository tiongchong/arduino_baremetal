#include <avr/io.h>
#include <avr/interrupt.h>

// Function to initialize Timer1 in CTC mode for 500ms delay
void timer1_init_500ms() {
    // Set CTC mode: WGM12 = 1, WGM13:0 = 0100
    TCCR1B |= (1 << WGM12);

    // Set prescaler to 1024: CS12 = 1, CS11 = 0, CS10 = 1
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);

    // Calculate OCR1A for 500ms:
    // OCR1A = (16,000,000 / (1024 * 2)) - 1 = 7812
    // So for 500ms: 7812 counts
    OCR1A = 7812;

    // Clear the timer
    TCNT1 = 0;
}

void timer1_delay_500ms() {
    // Clear the flag by writing 1 to it
    TIFR1 |= (1 << OCF1A);

    // Reset the counter
    TCNT1 = 0;

    // Wait until the compare match flag is set
    while (!(TIFR1 & (1 << OCF1A)));
}

int main(void) {
    // Set pin 5 of PORTB (digital pin 13) as output
    DDRB |= (1 << DDB5);

    // Initialize Timer1
    timer1_init_500ms();

    while (1) {
        // Toggle LED
        PORTB ^= (1 << PORTB5);

        // Delay using Timer1
        timer1_delay_500ms();
    }

    return 0;
}
