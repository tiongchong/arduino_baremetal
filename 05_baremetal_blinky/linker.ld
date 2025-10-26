/* Simple linker script for ATmega328P */

ENTRY(__vectors)

/* Memory layout (ATmega328P) */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x0000, LENGTH = 32K
  SRAM  (rw!x): ORIGIN = 0x0100, LENGTH = 0x0800  /* 2 KB (0x0100–0x08FF) */
}

/* Section placement */
SECTIONS
{
  .text :
  {
    *(.vectors)
    *(.text*)
    *(.rodata*)
  } > FLASH

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    *(.data*)
  } > SRAM

  .bss (NOLOAD) :
  {
    *(.bss*)
    *(COMMON)
  } > SRAM
}
