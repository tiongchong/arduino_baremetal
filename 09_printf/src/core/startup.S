/* startup.S — Defines interrupt vector table and reset entry for ATmega328P */

.section .vectors, "ax", @progbits
.global __vectors

__vectors:
    rjmp __init            ; Reset vector

    ; Unused interrupts → default handler
    .rept 25
        rjmp __bad_interrupt
    .endr

.text
.global __bad_interrupt
__bad_interrupt:
    rjmp __bad_interrupt   ; Infinite loop for unhandled interrupts
