/* startup.S — Interrupt vector table and minimal startup code for ATmega328P */

/* 
   The ATmega328P uses a fixed interrupt vector table starting at address 0x0000.
   Each vector is 2 bytes (one RJMP instruction). 
   On reset, the MCU jumps to the reset vector (first entry). 
   Other vectors correspond to hardware interrupts.
*/

    .section .vectors, "ax", @progbits  /* Place the vector table in the 'vectors' section
                                           'a' = allocatable, 'x' = executable, @progbits = normal data */
    .global __vectors                  /* Make vector table globally visible for the linker */

__vectors:
    rjmp __init       ; Reset vector → entry point for initialization (called from crt0.S)

    /* Interrupt vectors for ATmega328P */
    rjmp __bad_interrupt  ; INT0 - External Interrupt Request 0
    rjmp __bad_interrupt  ; INT1 - External Interrupt Request 1
    rjmp __bad_interrupt  ; PCINT0 - Pin Change Interrupt Request 0
    rjmp __bad_interrupt  ; PCINT1 - Pin Change Interrupt Request 1
    rjmp __bad_interrupt  ; PCINT2 - Pin Change Interrupt Request 2
    rjmp __bad_interrupt  ; WDT - Watchdog Timer
    rjmp __bad_interrupt  ; TIMER2_COMPA - Timer/Counter2 Compare Match A
    rjmp __bad_interrupt  ; TIMER2_COMPB - Timer/Counter2 Compare Match B
    rjmp __bad_interrupt  ; TIMER2_OVF - Timer/Counter2 Overflow
    rjmp __bad_interrupt  ; TIMER1_CAPT - Timer/Counter1 Capture Event
    rjmp __bad_interrupt  ; TIMER1_COMPA - Timer/Counter1 Compare Match A
    rjmp __bad_interrupt  ; TIMER1_COMPB - Timer/Counter1 Compare Match B
    rjmp __bad_interrupt  ; TIMER1_OVF - Timer/Counter1 Overflow
    rjmp __bad_interrupt  ; TIMER0_COMPA - Timer/Counter0 Compare Match A
    rjmp __bad_interrupt  ; TIMER0_COMPB - Timer/Counter0 Compare Match B
    rjmp __bad_interrupt  ; TIMER0_OVF - Timer/Counter0 Overflow
    rjmp __bad_interrupt  ; SPI_STC - SPI Serial Transfer Complete
    rjmp __bad_interrupt  ; USART_RX - USART Rx Complete
    rjmp __bad_interrupt  ; USART_UDRE - USART Data Register Empty
    rjmp __bad_interrupt  ; USART_TX - USART Tx Complete
    rjmp __bad_interrupt  ; ADC - ADC Conversion Complete
    rjmp __bad_interrupt  ; EE_READY - EEPROM Ready
    rjmp __bad_interrupt  ; ANALOG_COMP - Analog Comparator
    rjmp __bad_interrupt  ; TWI - Two-Wire Serial Interface
    rjmp __bad_interrupt  ; SPM_READY - Store Program Memory Ready

/* 
   __bad_interrupt: Default handler for all unimplemented interrupts.
   It loops infinitely to prevent undefined behavior if an interrupt fires unexpectedly.
*/

    .text
    .global __bad_interrupt
__bad_interrupt:
    rjmp __bad_interrupt   ; Infinite loop: traps MCU here if unhandled interrupt occurs

/* 
   __init: Initialization entry point for MCU after reset.
   This is called via the reset vector. Typically, it will jump to _start
   defined in crt0.S, which sets up the stack, initializes data/bss, and calls main().
*/

    .global __init
__init:
    rjmp _start             ; Jump to minimal C runtime startup code (_start) in crt0.S
