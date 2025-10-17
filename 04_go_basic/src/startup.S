// -----------------------------------------------------------------------------
// Interrupt Vector Table for AVR microcontroller (e.g., ATmega328P)
// -----------------------------------------------------------------------------
// This section defines the interrupt vector table, which is placed at the
// beginning of the program memory (FLASH). The microcontroller uses this table
// to jump to the appropriate interrupt service routine (ISR) when an interrupt
// occurs.
//
// Each entry is a relative jump instruction (rjmp) to the corresponding ISR.
// The reset vector must point to the startup/reset routine, which initializes
// the MCU and calls main().
//
// Unused interrupt vectors here are redirected to address 0 (rjmp 0),
// effectively pointing to the reset vector or an empty handler (could cause a
// reset or hang, depending on implementation).
//
// The vector table is placed in the `.vectors` section with attributes:
//  "a" (allocatable), @progbits (normal program code/data).
.section .vectors, "a", @progbits

// Make the __vectors symbol globally visible for the linker/startup code
.global __vectors

// Label marking the start of the vector table
__vectors:
    rjmp reset      // 0x0000 - Reset vector: jump to reset handler (startup code)
    rjmp 0          // 0x0002 - INT0 External Interrupt 0 vector (unused here)
    rjmp 0          // 0x0004 - INT1 External Interrupt 1 vector (unused)
    rjmp 0          // 0x0006 - PCINT0 Pin Change Interrupt 0 vector (unused)
    rjmp 0          // 0x0008 - PCINT1 Pin Change Interrupt 1 vector (unused)
    rjmp 0          // 0x000A - PCINT2 Pin Change Interrupt 2 vector (unused)
    rjmp 0          // 0x000C - Watchdog Timer Interrupt vector (unused)
    rjmp 0          // 0x000E - Timer/Counter2 Compare Match A (unused)
    rjmp 0          // 0x0010 - Timer/Counter2 Compare Match B (unused)
    rjmp 0          // 0x0012 - Timer/Counter2 Overflow (unused)
    rjmp 0          // 0x0014 - Timer/Counter1 Capture Event (unused)
    rjmp 0          // 0x0016 - Timer/Counter1 Compare Match A (unused)
    rjmp 0          // 0x0018 - Timer/Counter1 Compare Match B (unused)
    rjmp 0          // 0x001A - Timer/Counter1 Overflow (unused)
    rjmp 0          // 0x001C - Timer/Counter0 Compare Match A (unused)
    rjmp 0          // 0x001E - Timer/Counter0 Compare Match B (unused)
    rjmp 0          // 0x0020 - Timer/Counter0 Overflow (unused)
    rjmp 0          // 0x0022 - SPI Serial Transfer Complete (unused)
    rjmp 0          // 0x0024 - USART RX Complete (unused)
    rjmp 0          // 0x0026 - USART Data Register Empty (unused)
    rjmp 0          // 0x0028 - USART TX Complete (unused)
    rjmp 0          // 0x002A - ADC Conversion Complete (unused)
    rjmp 0          // 0x002C - EEPROM Ready (unused)
    rjmp 0          // 0x002E - Analog Comparator (unused)
    rjmp 0          // 0x0030 - Two-wire Serial Interface (TWI) (unused)
    rjmp 0          // 0x0032 - Store Program Memory Ready (unused)

// -----------------------------------------------------------------------------
// Reset handler - program entry point after MCU reset
// -----------------------------------------------------------------------------
.section .text
.global reset

reset:
    // Call the C runtime initialization routine
    // This function typically copies .data from FLASH to SRAM,
    // zeroes .bss section, and performs other setup before main()
    call _init

    // Call main program entry point (C main function)
    call main

    // If main() returns, enter an infinite loop to prevent MCU from
    // executing random instructions or resetting unintentionally.
hang:
    rjmp hang
