/* AVR linker script (ATmega328P style) */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x000000, LENGTH = 32K
  SRAM  (rw!x): ORIGIN = 0x800100, LENGTH = 2K
}

/* Default entry point */
ENTRY(__vectors)

SECTIONS
{
  /* Interrupt vectors */
  .vectors :
  {
    KEEP(*(.vectors))
  } > FLASH

  /* Program code */
  .text :
  {
    *(.text*)
    *(.progmem*)
    *(.init*)
    *(.fini*)
    KEEP(*(.ctors))
    KEEP(*(.dtors))
  } > FLASH

  /* Read-only data in flash */
  .rodata :
  {
    *(.rodata*)
  } > FLASH

  /* Initialized data (copied to SRAM) */
  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    __data_start = .;
    *(.data*)
    __data_end = .;
  } > SRAM

  __data_load_start = LOADADDR(.data);

  /* Zero-initialized data */
  .bss :
  {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } > SRAM

  /* Heap & stack symbols */
  __heap_start = __bss_end;
  __stack_top  = ORIGIN(SRAM) + LENGTH(SRAM);
}
