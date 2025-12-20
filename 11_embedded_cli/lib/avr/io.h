#ifndef IO_H
#define IO_H

#include "stdint.h"  // Include standard integer types for fixed width integers

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


// UART0 Register Addresses (from ATmega328P datasheet)
#define UCSR0A   (*(volatile uint8_t*)0xC0)
#define UCSR0B   (*(volatile uint8_t*)0xC1)
#define UCSR0C   (*(volatile uint8_t*)0xC2)

#define UBRR0L   (*(volatile uint8_t*)0xC4)
#define UBRR0H   (*(volatile uint8_t*)0xC5)

#define UDR0     (*(volatile uint8_t*)0xC6)

#define RXC0    7   // Receive Complete
#define TXC0    6   // Transmit Complete
#define UDRE0   5   // Data Register Empty
#define FE0     4   // Frame Error
#define DOR0    3   // Data Overrun
#define UPE0    2   // Parity Error
#define U2X0    1   // Double Transmission Speed
#define MPCM0   0   // Multi-processor Mode

#define RXCIE0  7   // RX Complete Interrupt Enable
#define TXCIE0  6   // TX Complete Interrupt Enable
#define UDRIE0  5   // Data Register Empty Interrupt Enable
#define RXEN0   4   // Receiver Enable
#define TXEN0   3   // Transmitter Enable
#define UCSZ02  2   // Character Size bit 2
#define RXB80   1   // Receive Data Bit 8
#define TXB80   0   // Transmit Data Bit 8

#define UMSEL01 7   // Mode Select 1
#define UMSEL00 6   // Mode Select 0
#define UPM01   5   // Parity Mode 1
#define UPM00   4   // Parity Mode 0
#define USBS0   3   // Stop Bit Select
#define UCSZ01  2   // Character Size bit 1
#define UCSZ00  1   // Character Size bit 0
#define UCPOL0  0   // Clock Polarity

// ---- Registers ----
#define UBRR0H   (*(volatile uint8_t*)0xC5)
#define UBRR0L   (*(volatile uint8_t*)0xC4)

#define UCSR0A   (*(volatile uint8_t*)0xC0)
#define UCSR0B   (*(volatile uint8_t*)0xC1)
#define UCSR0C   (*(volatile uint8_t*)0xC2)

#define UDR0     (*(volatile uint8_t*)0xC6)

// ---- Bit masks ----
#define UDRE0    5

#define TXEN0    3

#define UCSZ01   2
#define UCSZ00   1


#endif // IO_H
