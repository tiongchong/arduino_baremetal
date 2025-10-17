.section .text
.global _init

// -----------------------------------------------------------------------------
// _init: C runtime initialization routine
// -----------------------------------------------------------------------------
// This routine runs before the main() function in an embedded environment.
// Its purpose is to initialize the .data and .bss sections.
//
// .data section:
//   Contains initialized global/static variables.
//   The initial values are stored in FLASH (program memory).
//   These values need to be copied to SRAM (data memory) at startup.
//
// .bss section:
//   Contains zero-initialized global/static variables.
//   These need to be cleared (set to zero) at startup.
//
// Registers used:
//   r24:r25 - pointer to destination start in SRAM for .data or .bss
//   r26:r27 - pointer to destination end in SRAM for .data or .bss
//   r30:r31 (Z register) - pointer to source start in FLASH for .data
//   r0 - temporary register for zero value when clearing .bss

_init:
    ; Load start address of initial values for .data in FLASH into Z (r31:r30)
    ldi r30, lo8(__data_load_start)   ; Load lower 8 bits of __data_load_start address
    ldi r31, hi8(__data_load_start)   ; Load upper 8 bits of __data_load_start address

    ; Load start address of .data section in SRAM into X (r25:r24)
    ldi r24, lo8(__data_start)        ; Load lower 8 bits of __data_start address
    ldi r25, hi8(__data_start)        ; Load upper 8 bits of __data_start address

    ; Load end address of .data section in SRAM into Y (r27:r26)
    ldi r26, lo8(__data_end)          ; Load lower 8 bits of __data_end address
    ldi r27, hi8(__data_end)          ; Load upper 8 bits of __data_end address

copy_data:
    ; Compare current destination pointer (X: r25:r24) with end pointer (Y: r27:r26)
    cp r24, r26                      ; Compare low bytes
    cpc r25, r27                     ; Compare high bytes with carry
    brge clear_bss                   ; If X >= Y (copy done), branch to clear_bss

    ; Load byte from FLASH (Z register) into r0 and store it to SRAM (X register)
    ld r0, Z+                       ; Load byte from Z pointer, increment Z
    st X+, r0                       ; Store r0 into SRAM at X pointer, increment X
    rjmp copy_data                  ; Repeat until all .data bytes copied

clear_bss:
    ; Load start address of .bss section in SRAM into X (r25:r24)
    ldi r24, lo8(__bss_start)
    ldi r25, hi8(__bss_start)

    ; Load end address of .bss section in SRAM into Y (r27:r26)
    ldi r26, lo8(__bss_end)
    ldi r27, hi8(__bss_end)

    clr r0                         ; Clear r0 to zero - this will be used to zero memory

zero_bss:
    ; Compare current pointer (X) with end pointer (Y)
    cp r24, r26                    ; Compare low bytes
    cpc r25, r27                   ; Compare high bytes with carry
    brge init_done                 ; If X >= Y, all bytes cleared, branch to init_done

    st X+, r0                     ; Store zero into SRAM at X pointer, increment X
    rjmp zero_bss                 ; Repeat until all .bss bytes zeroed

init_done:
    ret                           ; Return from _init to caller (usually main startup code)
