/* ATmega328P bare-metal linker script */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x000000, LENGTH = 32K
  SRAM  (rw!x): ORIGIN = 0x800100, LENGTH = 2K
}

ENTRY(__vectors)

SECTIONS
{
  /* Interrupt vectors MUST start at 0x0000 */
  .vectors 0x0000 :
  {
    KEEP(*(.vectors))
    KEEP(*(.vectors.*))
  } > FLASH

  /* Default interrupt handler MUST be close (RJMP range) */
  .lowtext :
  {
    KEEP(*(__bad_interrupt))
    KEEP(*(.lowtext*))
  } > FLASH

  /* Main program code */
  .text :
  {
    *(.text*)
    *(.init*)
    *(.fini*)
    KEEP(*(.ctors))
    KEEP(*(.dtors))
  } > FLASH

  /* All read-only data in flash */
  .rodata :
  {
    *(.rodata*)
    *(.progmem*)
    *(.jumptables.gcc*)
  } > FLASH

  /* Initialized data (VMA=SRAM, LMA=FLASH) */
  .data : AT (ADDR(.rodata) + SIZEOF(.rodata))
  {
    __data_start = .;
    *(.data*)
    __data_end = .;
  } > SRAM

  __data_load_start = LOADADDR(.data);

  /* Zero-initialized data */
  .bss (NOLOAD) :
  {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } > SRAM

  /* Heap & stack */
  __heap_start = __bss_end;
  __heap_end   = ORIGIN(SRAM) + LENGTH(SRAM) - 1;
  __stack_top  = ORIGIN(SRAM) + LENGTH(SRAM);
}
