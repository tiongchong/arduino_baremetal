#include "avr/io.h" // your custom AVR I/O header 
// ----------------------------------------------------------------------------- 
// UART INIT / PUTCHAR FOR printf 
// ----------------------------------------------------------------------------- 
#define F_CPU 16000000UL // adjust based on actual clock 
#define BAUD 57600 
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1) 

void uart_init(void) { 
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8); 
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF); 
    UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable TX and RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1 
} 

void uart_putc(char c) { 
    while (!(UCSR0A & (1 << UDRE0))); 
    UDR0 = c; 
}

// receive one character over UART
char uart_getc(void) {
    while (!(UCSR0A & (1 << RXC0))); // wait until data is received
    return UDR0;                     // read received byte
}

int main(void) { 
    uart_init(); 

    uart_putc('>'); 
    uart_putc('\r'); 
    uart_putc('\n');
    
    char received;
    
    while(1) { 
        // Echo received character back
        received = uart_getc();
        uart_putc(received); 
        uart_putc('\r');
        uart_putc('\n'); 
    } 
}