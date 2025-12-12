/* crt0.S — Minimal C runtime for ATmega328P */

.global __init
.global main

.equ RAMEND, 0x08FF       ; End of SRAM for ATmega328P (2 KB)

.section .text
__init:
    ; Initialize stack pointer
    ldi r16, hi8(RAMEND)
    out 0x3E, r16          ; SPH
    ldi r16, lo8(RAMEND)
    out 0x3D, r16          ; SPL

    ; Call main()
    call main

1:  rjmp 1b                ; Loop forever if main returns
