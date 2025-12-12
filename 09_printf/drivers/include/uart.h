#ifndef UART_H
#define UART_H

#include "avr/io.h"
#include "config.h"

#define BAUD 57600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void);

void printf(const char *fmt, ...);

#endif // UART_H