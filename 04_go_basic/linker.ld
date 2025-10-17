ENTRY(__vectors)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x0000, LENGTH = 32K
    RAM (rw)   : ORIGIN = 0x0100, LENGTH = 2K
}

SECTIONS
{
    .vectors :
    {
        KEEP(*(.vectors))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
    } > FLASH

    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        __data_start = .;
        *(.data*)
        __data_end = .;
    } > RAM

    .data_load :
    {
        __data_load_start = LOADADDR(.data);
    } > FLASH

    .bss :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > RAM
}
