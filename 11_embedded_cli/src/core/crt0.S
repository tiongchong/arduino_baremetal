/* crt0.S – AVR startup */

#include "avr/io.h"

.section .vectors, "ax", @progbits
.global __vectors
__vectors:
  rjmp reset              /* Reset vector */
  rjmp __bad_interrupt    /* INT0 */
  rjmp __bad_interrupt    /* INT1 */
  rjmp __bad_interrupt    /* PCINT0 */
  rjmp __bad_interrupt    /* PCINT1 */
  rjmp __bad_interrupt    /* PCINT2 */
  rjmp __bad_interrupt    /* WDT */
  rjmp __bad_interrupt    /* TIMER2_COMPA */
  rjmp __bad_interrupt    /* TIMER2_COMPB */
  rjmp __bad_interrupt    /* TIMER2_OVF */
  rjmp __bad_interrupt    /* TIMER1_CAPT */
  rjmp __bad_interrupt    /* TIMER1_COMPA */
  rjmp __bad_interrupt    /* TIMER1_COMPB */
  rjmp __bad_interrupt    /* TIMER1_OVF */
  rjmp __bad_interrupt    /* TIMER0_COMPA */
  rjmp __bad_interrupt    /* TIMER0_COMPB */
  rjmp __bad_interrupt    /* TIMER0_OVF */
  rjmp __bad_interrupt    /* SPI */
  rjmp __bad_interrupt    /* USART_RX */
  rjmp __bad_interrupt    /* USART_UDRE */
  rjmp __bad_interrupt    /* USART_TX */
  rjmp __bad_interrupt    /* ADC */
  rjmp __bad_interrupt    /* EE_READY */
  rjmp __bad_interrupt    /* ANALOG_COMP */
  rjmp __bad_interrupt    /* TWI */
  rjmp __bad_interrupt    /* SPM_READY */

.section .text
.global reset
reset:
  cli

  /* Set stack pointer */
  ldi r16, hi8(__stack_top)
  out _SFR_IO_ADDR(SPH), r16
  ldi r16, lo8(__stack_top)
  out _SFR_IO_ADDR(SPL), r16

  /* Copy .data from flash to SRAM */
  ldi r30, lo8(__data_load_start)
  ldi r31, hi8(__data_load_start)
  ldi r26, lo8(__data_start)
  ldi r27, hi8(__data_start)
  ldi r24, lo8(__data_end)
  ldi r25, hi8(__data_end)
copy_data:
  cp r26, r24
  cpc r27, r25
  breq clear_bss
  lpm r0, Z+
  st X+, r0
  rjmp copy_data

clear_bss:
  ldi r26, lo8(__bss_start)
  ldi r27, hi8(__bss_start)
  ldi r24, lo8(__bss_end)
  ldi r25, hi8(__bss_end)
  clr r0
clear_loop:
  cp r26, r24
  cpc r27, r25
  breq start_main
  st X+, r0
  rjmp clear_loop

start_main:
  sei
  rcall main

hang:
  rjmp hang

.section .lowtext,"ax",@progbits
.global __bad_interrupt
__bad_interrupt:
    rjmp __bad_interrupt
