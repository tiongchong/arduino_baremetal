/* ATmega328P Linker Script — Full Detailed Explanation */

/* 
   ENTRY(__vectors):
   ----------------
   Specifies the entry point for the program. 
   On reset, the MCU will start execution at the address of __vectors (the vector table defined in startup.S). 
   This tells the linker which symbol corresponds to the start of execution.
*/
ENTRY(__vectors)

MEMORY
{
    /* Flash memory: 32 KB, read/execute (rx) */
    flash (rx)    : ORIGIN = 0x00000000, LENGTH = 32K
    
    /* SRAM: 2 KB, read/write, non-executable (rw!x) */
    ram   (rw!x)  : ORIGIN = 0x00800100, LENGTH = 2K
    
    /* EEPROM: 1 KB, read/write, non-executable (rw!x) */
    eeprom (rw!x) : ORIGIN = 0x00810000, LENGTH = 1K
}

/* 
   SECTIONS:
   ---------
   Defines how different program sections (.text, .data, .bss, etc.) are placed in memory.
   The linker will combine all input object files (.o) according to these rules.
*/
SECTIONS
{
    /* 
       .text section — program code and read-only data
       -----------------------------------------------
       Typically placed in flash because it is non-volatile and executable.
    */
    .text :
    {
        *(.vectors)       /* Include the interrupt vector table from startup.S */
        *(.text)          /* Include main program code sections from object files */
        *(.text.*)        /* Include any sub-sections like .text.foo or .text.bar */
        *(.rodata)        /* Read-only data (constants, string literals) */
        *(.rodata*)       /* Any other read-only subsections, e.g., .rodata.strings */
        . = ALIGN(2);     /* Align the current location to 2-byte boundary (required by AVR RJMP) */
        _etext = .;       /* Define symbol _etext = end of .text section */
    } > flash             /* Place this entire section in flash memory */

    /* 
       .data section — initialized global/static variables
       ---------------------------------------------------
       Resides in SRAM at runtime, but initialized values are stored in flash.
       The linker generates a copy of the initial values in flash (AT > flash)
    */
    .data : 
    {
        _data = .;        /* Start of .data in SRAM */
        *(.data)          /* Include initialized variables */
        *(.data.*)        /* Include any data subsections */
        . = ALIGN(2);     /* Align to 2-byte boundary */
        _edata = .;       /* End of .data section */
    } > ram AT > flash      /* Load in flash but run in SRAM */

    /* 
       .bss section — uninitialized global/static variables
       -----------------------------------------------------
       Contains zero-initialized variables. At runtime, startup code clears this memory.
       Stored only in SRAM; no need to occupy flash.
    */
    .bss :
    {
        _bss = .;         /* Start of .bss */
        *(.bss)           /* Include uninitialized variables */
        *(.bss.*)         /* Include subsections of .bss */
        *(COMMON)         /* Include common symbols (uninitialized globals) */
        . = ALIGN(2);     /* Align to 2-byte boundary */
        _ebss = .;        /* End of .bss */
    } > ram

    /* 
       _end symbol — marks the end of all memory used by program
       Useful for runtime calculations (e.g., heap start = _end)
    */
    _end = .;
}
