/**
 *
 * @file software_pwm.c
 *
 *
 * This program controls an LED connected to PB5 (Arduino pin 13)
 * and makes it “breathe,”
 * i.e., smoothly increase and decrease in brightness
 *       like a breathing pattern.
 *
 * It does software PWM by turning the pin HIGH and LOW manually
 * with precise delays to simulate analog brightness.
 *
 * The timing uses Timer1 in CTC mode for accurate 20-microsecond delays.
 *
 * The brightness changes with a smooth sine-based curve,
 * representing inhale and exhale.
 *
 * Randomness is added via a simple pseudo-random generator
 * to vary the breathing rate and brightness a bit
 * to make the effect look more natural.
 *
 * The main loop updates the brightness every ~5 milliseconds,
 * cycling the breath pattern continuously.
 *
 */

#define F_CPU 16000000UL // Define CPU clock speed as 16 MHz

#include <avr/interrupt.h> // Interrupt handling (not really used here)
#include <avr/io.h>        // AVR device-specific IO definitions
#include <math.h>          // Math functions like sin(), powf()

// Define the PWM pin and its associated port and direction register
#define PWM_PIN PB5 // Pin 5 on Port B (Arduino Uno digital pin 13)
#define PWM_PORT PORTB
#define PWM_DDR DDRB

// Breathing frequency in Hz (~0.16667 Hz means one breath every 6 seconds)
#define BREATH_FREQUENCY 0.16667f

// Initialize PWM pin as output
void pwm_pin_init() {
    PWM_DDR |= (1 << PWM_PIN); // Set PWM_PIN bit in DDRB to 1 => output
}

// Initialize Timer1 to create a delay of approximately 20 microseconds
void timer1_init_20us() {
    TCCR1A = 0; // Clear Timer/Counter1 Control Registers
    TCCR1B = 0;
    TCNT1 = 0; // Reset Timer1 counter value

    OCR1A = 319;            // Set compare register for 20us delay:
                            // Calculation:
                            // Timer clock frequency = 16 MHz (no prescaler)
                            // Timer tick time = 1/16,000,000 = 0.0625 us
                            // Number of ticks for 20 us = 20 / 0.0625 = 320
                            // OCR1A = 319 (because timer counts from 0)
    TCCR1B |= (1 << WGM12); // Configure Timer1 in CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << CS10);  // No prescaling, timer runs at full CPU clock speed
}

// Busy wait for 20 microseconds using Timer1 compare match flag
void timer1_delay_20us() {
    TCNT1 = 0;             // Reset timer count
    TIFR1 |= (1 << OCF1A); // Clear Output Compare Flag by writing 1
    while (!(TIFR1 & (1 << OCF1A)))
        ; // Wait until compare match occurs
}

// Generate a pseudo-random float between 0 and 1 using a simple XOR shift
// algorithm
static float rand_float() {
    static uint16_t seed = 0xACE1u; // Static seed value, retains value between calls
    seed ^= seed << 7;              // XOR shift operations to generate new seed
    seed ^= seed >> 9;
    seed ^= seed << 8;
    // Normalize the 16-bit result to float between 0 and 1
    return (float)(seed & 0xFFFF) / 65535.0f;
}

// Smooth random function to produce a value gradually changing over time
// 'prev' is the previous value, 'alpha' controls how fast the value changes
float smooth_rand(float prev, float alpha) {
    // Move previous value a bit closer to a new random value
    return prev + alpha * (rand_float() - prev);
}

#define MAX_BRIGHTNESS_BASE 0.100f // Base max brightness (scaled to duty cycle)
#define MIN_BRIGHTNESS_BASE 0.005f // Base min brightness

// Calculate the PWM duty cycle (0-255) based on a "breathing" pattern at time
// 't' rand_state1, rand_state2, rand_state3 hold smooth random states to vary
// the breath
uint8_t get_breath_duty(float t, float *rand_state1, float *rand_state2, float *rand_state3) {
    // Calculate base breath period from frequency (seconds per breath cycle)
    float breath_period_base = 1.0f / BREATH_FREQUENCY;

    // Introduce a slight random variation to the breath period (~±5%)
    float breath_period = breath_period_base * (0.95f + 0.05f * smooth_rand(*rand_state1, 0.01f));
    *rand_state1 = breath_period; // Update rand_state1 with new breath_period value

    float half_period = breath_period / 2.0f;

    // For initial time less than breath period, turn LED off (duty = 0)
    if (t < breath_period)
        return 0;

    // Time within current breath cycle, modulo breath period to wrap time
    float adjusted_t = t - breath_period;
    float inhale_duration = 0.7f * half_period; // Inhale takes 70% of half period
    float exhale_duration = breath_period - inhale_duration;

    // Phase time cycles between 0 and breath_period
    float phase_time = fmod(adjusted_t, breath_period);

    // Slight random variation in max brightness (~±10%)
    float max_brightness = MAX_BRIGHTNESS_BASE * (0.9f + 0.1f * smooth_rand(*rand_state2, 0.02f));
    *rand_state2 = max_brightness;

    // Slight random variation in min brightness (~±10%)
    float min_brightness = MIN_BRIGHTNESS_BASE * (0.8f + 0.1f * smooth_rand(*rand_state3, 0.02f));
    *rand_state3 = min_brightness;

    float breath; // Will hold the normalized brightness value

    if (phase_time < inhale_duration) {
        // Inhale phase: brightness rises smoothly with a sine curve
        float p = phase_time / inhale_duration; // Progress from 0 to 1 during inhale
        float s = sin(M_PI * p);                // Sinusoidal easing from 0 to 1
        breath = powf(s, 2.0f);                 // Square to smooth curve more
        breath *= (0.95f + 0.1f * smooth_rand(*rand_state3, 0.05f)); // Slight randomness
    } else {
        // Exhale phase: brightness decreases smoothly, inverted sine curve
        float p = (phase_time - inhale_duration) / exhale_duration;
        float s = sin(M_PI * p + M_PI); // Sine wave shifted by PI (starts at 0, ends at 0)
        breath = -powf(-s, 3.0f);       // Cubic easing for a smoother curve
        breath *= (0.95f + 0.1f * smooth_rand(*rand_state3, 0.05f));
    }

    // Map breath from [-1, 1] to [0, 1]
    breath = (breath + 1.0f) / 2.0f;

    // Scale breath to between min and max brightness
    breath = breath * (max_brightness - min_brightness) + min_brightness;

    // Convert brightness (0.0-1.0) to PWM duty cycle (0-255)
    return (uint8_t)(breath * 255.0f);
}

// Software PWM routine: generates PWM by turning pin on/off manually
// duty = 0 (off) to 255 (fully on)
void software_pwm(uint8_t duty) {
    // Loop 255 times for 8-bit resolution PWM
    for (uint8_t i = 0; i < 255; i++) {
        if (i < duty)
            PWM_PORT |= (1 << PWM_PIN); // Turn pin HIGH (LED on)
        else
            PWM_PORT &= ~(1 << PWM_PIN); // Turn pin LOW (LED off)

        timer1_delay_20us(); // Wait ~20 microseconds per step
    }
}

// Main program loop
int main(void) {
    pwm_pin_init();     // Setup PWM pin as output
    timer1_init_20us(); // Initialize timer for delays

    float time = 0.0f;              // Elapsed time in seconds (simulated)
    const float time_step = 0.005f; // Time step increment per PWM cycle (5 ms)

    // States for smooth random generators
    float rand_state1 = 0.0f, rand_state2 = 0.0f, rand_state3 = 0.0f;

    while (1) {
        // Calculate PWM duty cycle for the current time
        uint8_t duty = get_breath_duty(time, &rand_state1, &rand_state2, &rand_state3);

        // Output PWM signal with calculated duty cycle (software PWM)
        software_pwm(duty);

        // Increment time by time_step seconds
        time += time_step;
    }
}
