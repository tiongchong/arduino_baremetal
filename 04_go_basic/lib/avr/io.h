#ifndef IO_H
#define IO_H

#include <stdint.h>  // Include standard integer types for fixed width integers

// -----------------------------------------------------------------------------
// Register I/O macros
// -----------------------------------------------------------------------------
// Macro to define an 8-bit I/O register at a specific address.
// volatile keyword ensures the compiler does not optimize away accesses to these
// registers since their values can change outside program control (hardware).
// Casting the address to a pointer to volatile uint8_t and dereferencing it
// allows direct access to the hardware register.
//
// Usage example:
//   uint8_t port_value = PORTB;  // Read the PORTB register
//   PORTB = 0xFF;                // Write to PORTB register
#define _SFR_IO8(addr) (*(volatile uint8_t *)(addr))

// -----------------------------------------------------------------------------
// ATmega328P I/O register base addresses
// -----------------------------------------------------------------------------
// These addresses correspond to the hardware registers for ports B, C, and D
// on the ATmega328P microcontroller. The addresses come directly from the
// ATmega328P datasheet. Each port has three main registers:
//
// - PORTx: Data Register — writing here sets the output value on pins when pins
//   are configured as outputs. Reading returns the last value written.
//
// - DDRx: Data Direction Register — configuring pins as inputs or outputs.
//   Bit set to 1 configures corresponding pin as output, 0 as input.
//
// - PINx: Input Pins Address — reading returns the current logical value on
//   the pins (actual voltage level), regardless of whether configured as input
//   or output.
//
// Note: Direct register access via these macros is common in embedded C for AVR.
//
// Port B Registers (digital pins 8 to 13 typically):
#define PORTB _SFR_IO8(0x25)  // Data Register for Port B
#define DDRB  _SFR_IO8(0x24)  // Data Direction Register for Port B
#define PINB  _SFR_IO8(0x23)  // Input Pins Register for Port B

// Port C Registers (analog input pins 0 to 5):
#define PORTC _SFR_IO8(0x28)  // Data Register for Port C
#define DDRC  _SFR_IO8(0x27)  // Data Direction Register for Port C
#define PINC  _SFR_IO8(0x26)  // Input Pins Register for Port C

// Port D Registers (digital pins 0 to 7):
#define PORTD _SFR_IO8(0x2B)  // Data Register for Port D
#define DDRD  _SFR_IO8(0x2A)  // Data Direction Register for Port D
#define PIND  _SFR_IO8(0x29)  // Input Pins Register for Port D

// -----------------------------------------------------------------------------
// Bit position macros for port pins
// -----------------------------------------------------------------------------
// These macros define the bit positions of each pin within the port registers.
// Using these symbolic names improves code readability and maintainability.
// For example, to set pin 0 of PORTB high:
//   PORTB |= (1 << PB0);
//
// Pin numbering corresponds to bit positions in the register (0 to 7).
// This aligns with the hardware pin layout as defined in the datasheet.

// Port B pins (digital pins 8 to 13)
#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6
#define PB7 7

// Port C pins (analog input pins 0 to 6, though PC6 is often reset pin)
#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC4 4
#define PC5 5
#define PC6 6

// Port D pins (digital pins 0 to 7)
#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3
#define PD4 4
#define PD5 5
#define PD6 6
#define PD7 7

#endif // IO_H
