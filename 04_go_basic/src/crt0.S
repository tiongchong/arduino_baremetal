/* ==================================================================================================
 * crt0.S — C Runtime Startup Code for ATmega328P
 * --------------------------------------------------------------------------------------------------
 * PURPOSE:
 *   This file defines the low-level startup routine (_init) that runs immediately after a reset.
 *   It is responsible for preparing the RAM memory before calling 'main()' in a bare-metal
 *   (no operating system) environment.
 *
 * OVERVIEW OF STARTUP SEQUENCE:
 *   1. Copy initialized data (.data section) from FLASH → SRAM
 *   2. Clear uninitialized data (.bss section) in SRAM (set to zero)
 *   3. Continue to the main program (either by returning or jumping to main)
 *
 * RELATIONSHIP TO LINKER SCRIPT:
 *   This code depends on symbols provided by the linker script:
 *
 *       __data_load_start  - Start address of .data values in FLASH
 *       __data_start       - Start of .data in SRAM
 *       __data_end         - End of .data in SRAM
 *       __bss_start        - Start of .bss in SRAM
 *       __bss_end          - End of .bss in SRAM
 *
 *   These symbols are defined in the linker script’s SECTIONS block.
 *
 * REGISTER USAGE:
 *   Z  (r31:r30) — Source pointer in FLASH (for copying .data)
 *   X  (r25:r24) — Destination pointer in SRAM (for copying/clearing data)
 *   Y  (r27:r26) — End pointer in SRAM (for range comparison)
 *   r0           — Temporary register used for data transfer or zero fill
 *
 * NOTES:
 *   - The ATmega328P executes code directly from FLASH, so LPM (Load Program Memory) is used
 *     to read constants from program memory.
 *   - The CPU starts execution at the reset vector (0x0000 in FLASH), which typically jumps
 *     to this _init routine.
 *
 * COMPATIBILITY:
 *   - Assembles with avr-gcc / avr-as
 *   - Designed for 8-bit AVR devices with <= 64K address space (like ATmega328P)
 *
 * ==================================================================================================
 */

.section .text
.global _init

// -----------------------------------------------------------------------------
// _init — C Runtime Initialization
// -----------------------------------------------------------------------------
// This function initializes RAM before main() is called.
//
// Steps performed:
//   1. Copy initialized variables (.data) from FLASH → SRAM
//   2. Clear zero-initialized variables (.bss) in SRAM
//   3. Return to caller (usually a reset handler that later calls main())
//
// The linker and startup code ensure this executes immediately after reset.
// -----------------------------------------------------------------------------
_init:
    /* ----------------------------------------------------------------------------------------------
     * Step 1: Initialize .data section
     * ----------------------------------------------------------------------------------------------
     * Load initialized global/static variables into SRAM.
     *
     * In AVR, variables with initial values (e.g. int x = 5;) have those
     * initial bytes stored in FLASH. The startup code copies them to their
     * corresponding RAM locations before program execution begins.
     */

    ; Load FLASH address (source) into Z register (r31:r30)
    ldi r30, lo8(__data_load_start)      ; Lower byte of source address in FLASH
    ldi r31, hi8(__data_load_start)      ; Upper byte of source address in FLASH

    ; Load SRAM start address (destination) into X register (r25:r24)
    ldi r24, lo8(__data_start)           ; Lower byte of destination in SRAM
    ldi r25, hi8(__data_start)           ; Upper byte of destination in SRAM

    ; Load SRAM end address into Y register (r27:r26)
    ldi r26, lo8(__data_end)             ; Lower byte of end address
    ldi r27, hi8(__data_end)             ; Upper byte of end address

copy_data:
    ; Compare X (current dest) with Y (end)
    cp  r24, r26                         ; Compare low bytes
    cpc r25, r27                         ; Compare high bytes with carry
    brge clear_bss                       ; If X >= Y, all bytes copied, jump to .bss clearing

    ; Copy a single byte from FLASH to SRAM
    lpm r0, Z+                           ; Load from program memory (FLASH) via Z, auto-increment Z
    st  X+, r0                           ; Store to SRAM via X, auto-increment X

    rjmp copy_data                       ; Repeat until all .data copied


    /* ----------------------------------------------------------------------------------------------
     * Step 2: Clear .bss section
     * ----------------------------------------------------------------------------------------------
     * Variables that are not explicitly initialized (e.g. 'int counter;')
     * are placed in .bss and must start as zero.
     *
     * This loop writes zero to every byte in the .bss range.
     */

clear_bss:
    ; Load start and end addresses of .bss into X and Y
    ldi r24, lo8(__bss_start)
    ldi r25, hi8(__bss_start)
    ldi r26, lo8(__bss_end)
    ldi r27, hi8(__bss_end)

    clr r0                               ; r0 = 0 (used for zero-fill)

zero_bss:
    cp  r24, r26                         ; Compare low bytes
    cpc r25, r27                         ; Compare high bytes with carry
    brge init_done                       ; If start >= end, done clearing

    st  X+, r0                           ; Store zero to SRAM, increment pointer
    rjmp zero_bss                        ; Continue until end reached


    /* ----------------------------------------------------------------------------------------------
     * Step 3: Continue to main program
     * ----------------------------------------------------------------------------------------------
     * Depending on integration style:
     *   - If called as a subroutine: 'ret' returns to caller (e.g. reset handler)
     *   - If this _init is the reset entry point: replace 'ret' with 'rjmp main'
     */

init_done:
    ; rjmp main                          ; Uncomment if _init is the reset entry point
    ret                                  ; Return to caller (usually startup/entry vector)


// ==================================================================================================
// End of crt0.S
// --------------------------------------------------------------------------------------------------
// SUMMARY:
//   This startup code ensures that all C global/static variables are correctly prepared
//   before main() executes, fulfilling the C runtime requirements.
//
//   Without this, variables might contain garbage values, leading to undefined behavior.
//
// MEMORY FLOW SUMMARY:
//
//   FLASH (program memory):
//     [ .vectors | .text | .rodata | .data (load image) ]
//
//   SRAM (data memory after init):
//     [ .data (copied from FLASH) | .bss (zeroed) | stack grows downward ]
//
// --------------------------------------------------------------------------------------------------
// EXAMPLE RESET VECTOR LINKAGE:
//
//   .section .vectors, "ax"
//   .global __vectors
//   __vectors:
//       rjmp _init          ; Reset vector jumps to _init
//       rjmp 0              ; Other interrupt vectors (placeholders)
//
// --------------------------------------------------------------------------------------------------
// This file is part of a minimal bare-metal runtime environment for AVR microcontrollers.
// ==================================================================================================
