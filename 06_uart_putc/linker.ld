/* ATmega328P Linker Script
 * Flash (program memory):  0x0000 - 0x7FFF (32 KB)
 * SRAM  (data memory):     0x0100 - 0x08FF (2 KB)
 */

MEMORY
{
    text   (rx)  : ORIGIN = 0x0000, LENGTH = 32K
    data   (rw!x): ORIGIN = 0x0100, LENGTH = 2K
}

/* Entry point: Reset handler defined in startup code */
ENTRY(__vectors)

SECTIONS
{
    /* --------------------------------------------------------------------
     * FLASH SECTION (code + constants)
     * ------------------------------------------------------------------*/
    .text :
    {
        KEEP(*(.vectors))        /* interrupt vector table */
        *(.text*)                /* program code */
        *(.rodata*)              /* read-only data */
        *(.progmem*)             /* AVR GCC puts PROGMEM here */
        _etext = .;              /* end of text */
    } > text

    /* --------------------------------------------------------------------
     * DATA SECTION (flash → sram)
     * ------------------------------------------------------------------*/
    .data :
    {
        _data_start = .;
        *(.data*)                /* initialized variables */
        _data_end = .;
    } > data AT > text  /* load in flash, run in sram */

    /* Symbols for copying ROM→RAM */
    _data_load_start = LOADADDR(.data);
    _data_load_end   = LOADADDR(.data) + SIZEOF(.data);

    /* --------------------------------------------------------------------
     * BSS SECTION (zero-init in SRAM)
     * ------------------------------------------------------------------*/
    .bss (NOLOAD) :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > data

    /* --------------------------------------------------------------------
     * NOINIT SECTION (not initialized on reset)
     * ------------------------------------------------------------------*/
    .noinit (NOLOAD) :
    {
        *(.noinit*)
    } > data

    /* --------------------------------------------------------------------
     * END OF RAM (for stack pointer)
     * ------------------------------------------------------------------*/
    _end = .;
    PROVIDE(__stack = ORIGIN(data) + LENGTH(data) - 1);
}
