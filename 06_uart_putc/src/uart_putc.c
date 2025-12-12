#include "avr/io.h" // your custom AVR I/O header 
// ----------------------------------------------------------------------------- 
// UART INIT / PUTCHAR FOR printf 
// ----------------------------------------------------------------------------- 
#define F_CPU 16000000UL // adjust based on actual clock 
#define BAUD 9600 
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1) 

void uart_init(void) { 
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8); 
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF); 
    UCSR0B = (1 << TXEN0); // Enable TX only 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1 
} 

void uart_putc(char c) { 
    while (!(UCSR0A & (1 << UDRE0))); 
    UDR0 = c; 
}

int main(void) { 
    uart_init(); 
    while(1) { 
        uart_putc('A'); 
        uart_putc('\r'); 
        uart_putc('\n'); 
    } 
}