/* ==================================================================================================
 * ATmega328P Linker Script
 * --------------------------------------------------------------------------------------------------
 * This linker script defines how program sections (code, data, constants, etc.) are arranged in memory
 * for the AVR ATmega328P microcontroller.
 *
 * The ATmega328P has two main memory areas:
 *   - FLASH (program memory, non-volatile, 32 KB total)
 *   - SRAM (data memory, volatile, 2 KB total)
 *
 * This script tells the linker:
 *   - where each section (vectors, code, constants, data, BSS) should go,
 *   - how runtime memory is initialized (especially copying .data from FLASH to RAM),
 *   - and defines symbols used during C startup.
 *
 * These symbols and memory maps are essential for correct program startup (crt0), initialization,
 * and normal execution on bare-metal AVR systems.
 * ==================================================================================================
 */

/* --------------------------------------------------------------------------------------------------
 * ENTRY point symbol
 * --------------------------------------------------------------------------------------------------
 * ENTRY() defines the program's starting symbol. Here, "__vectors" is used, which is the vector table
 * containing the reset vector (program entry point) and interrupt vectors.
 *
 * On the ATmega328P, the reset vector (address 0x0000 in FLASH) contains a jump instruction that
 * transfers control to the main startup code (usually in crt0).
 */
ENTRY(__vectors)


/* --------------------------------------------------------------------------------------------------
 * MEMORY regions
 * --------------------------------------------------------------------------------------------------
 * The MEMORY block defines physical memory areas available on the ATmega328P.
 * Each region has:
 *   - a name (used later in > REGION_NAME syntax)
 *   - permissions (r = read, w = write, x = execute)
 *   - an origin (starting address)
 *   - a length (size in bytes)
 *
 * FLASH — 32 KB starting at 0x0000
 * RAM   — 2 KB starting at 0x0100 (the first 0x100 bytes are reserved for registers and I/O)
 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x0000, LENGTH = 32K   /* Program memory (non-volatile) */
    RAM (rw)   : ORIGIN = 0x0100, LENGTH = 2K    /* SRAM (volatile data memory) */
}


/* --------------------------------------------------------------------------------------------------
 * SECTIONS definition
 * --------------------------------------------------------------------------------------------------
 * This section defines how the compiler-generated sections (from object files) are mapped into memory.
 * Each block corresponds to a standard section type: .vectors, .text, .data, .bss, etc.
 * The linker combines input sections (from .o files) into these output sections.
 */
SECTIONS
{
    /* ----------------------------------------------------------------------------------------------
     * .vectors section
     * ----------------------------------------------------------------------------------------------
     * The interrupt vector table resides at the very beginning of FLASH memory.
     * It contains jump instructions to each interrupt service routine.
     *
     * KEEP() ensures this section is not discarded by the linker’s garbage collector even if no
     * references to it exist (since it must always be present).
     */
    .vectors :
    {
        KEEP(*(.vectors))     /* Keep all input sections named .vectors */
    } > FLASH                 /* Place in FLASH region */


    /* ----------------------------------------------------------------------------------------------
     * .text section
     * ----------------------------------------------------------------------------------------------
     * The .text section contains all executable code (functions) and read-only constants (.rodata).
     * This section is placed in FLASH since AVR executes directly from program memory.
     */
    .text :
    {
        *(.text*)             /* All code (functions, ISR, etc.) */
        *(.rodata*)           /* Read-only constants, e.g., string literals */
    } > FLASH                 /* Place in FLASH */


    /* ----------------------------------------------------------------------------------------------
     * .data section (initialized variables)
     * ----------------------------------------------------------------------------------------------
     * The .data section holds global/static variables that have initial (non-zero) values.
     *
     * On embedded systems like the ATmega328P:
     *   - The *initial values* are stored in FLASH (since SRAM is empty on reset).
     *   - At startup, the C runtime (crt0) copies them from FLASH to their RAM addresses.
     *
     * The 'AT (ADDR(.text) + SIZEOF(.text))' tells the linker to *load* this data immediately
     * following the .text section in FLASH. However, it will *execute from* RAM at runtime.
     */
    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        __data_start = .;     /* Symbol marking start of .data in RAM */
        *(.data*)             /* All initialized data sections */
        __data_end = .;       /* Symbol marking end of .data in RAM */
    } > RAM                   /* Will reside in RAM at runtime */


    /* ----------------------------------------------------------------------------------------------
     * .data_load section (helper symbol)
     * ----------------------------------------------------------------------------------------------
     * This defines a symbol used by startup code to locate the initial values for .data in FLASH.
     * The linker’s LOADADDR() function gives the FLASH address where .data is stored before copying.
     */
    .data_load :
    {
        __data_load_start = LOADADDR(.data);  /* FLASH address of .data initialization values */
    } > FLASH


    /* ----------------------------------------------------------------------------------------------
     * .bss section (zero-initialized variables)
     * ----------------------------------------------------------------------------------------------
     * The .bss section holds global/static variables initialized to zero (or uninitialized).
     * These do not need to be stored in FLASH because they can be simply cleared to zero at startup.
     * The C runtime (crt0) clears this section in RAM before main() runs.
     */
    .bss :
    {
        __bss_start = .;      /* Symbol marking start of zero-initialized data */
        *(.bss*)              /* All uninitialized or zeroed data */
        *(COMMON)             /* Common symbols (uninitialized global vars) */
        __bss_end = .;        /* Symbol marking end of .bss */
    } > RAM                   /* Stored in RAM */
}

/* ==================================================================================================
 * END OF LINKER SCRIPT
 * --------------------------------------------------------------------------------------------------
 * Summary of symbol meanings:
 *
 *   __vectors           - Entry point / interrupt vector table
 *   __data_start/end    - RAM range for initialized variables
 *   __data_load_start   - FLASH location of .data initialization values
 *   __bss_start/end     - RAM range for uninitialized variables
 *
 * These symbols are used in the C startup routine to:
 *   1. Copy .data from FLASH → RAM
 *   2. Zero out .bss
 *   3. Then call main()
 *
 * Example startup (pseudo-code):
 *   copy_memory(__data_load_start, __data_start, __data_end - __data_start);
 *   zero_memory(__bss_start, __bss_end - __bss_start);
 *   main();
 * ==================================================================================================
 */
