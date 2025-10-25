.global _start        /* Make the _start label globally visible so the linker can use it as the entry point */
.global main          /* Make the main function visible to the linker */

 /* Hardware definitions */
.equ RAMEND, 0x08FF  /* Define the end of SRAM for ATmega328P. 
                        The ATmega328P has 2 KB of SRAM (0x0800 to 0x08FF). 
                        This will be used to initialize the stack pointer. */

.section .vectors
    .word RAMEND     /* First word: initial value for the stack pointer (SP). 
                        On reset, the CPU loads SP from address 0x0000 in flash. */
    .word _start     /* Second word: reset vector. When the MCU resets, execution starts at _start */
    .org 0x26        /* Skip the rest of the interrupt vector table. 
                        ATmega328P has vectors up to 0x1F (32 bytes) for standard interrupts.
                        .org 0x26 ensures the following code is placed after the vector table. */

.section .text
_start:
    /* Initialize stack pointer */
    ldi r16, hi8(RAMEND)  /* Load high byte of RAMEND into register r16 */
    out 0x3e, r16         /* Write to SPH (Stack Pointer High) register at I/O address 0x3E */

    ldi r16, lo8(RAMEND)  /* Load low byte of RAMEND into r16 */
    out 0x3d, r16         /* Write to SPL (Stack Pointer Low) register at I/O address 0x3D */

    /* Call main function */
    call main             /* Jump to main function after basic initialization */

1:  rjmp 1b               /* Infinite loop to prevent the MCU from running into undefined memory 
                            if main returns. 
                            The "1b" means "jump back to the previous label '1'" */

