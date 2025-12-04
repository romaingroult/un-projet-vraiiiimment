#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "ring_buffer.h"

static uint8_t rx_buffer_data[RX_BUFFER_SIZE];
static struct ring_buffer rx_buffer;

void UART__init(void) {
    uint16_t ubrr = MYUBRR;

    UCSR0A = (1 << U2X0);
    ubrr = (F_CPU / 8 / BAUD) - 1;
    
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    ring_buffer__init(&rx_buffer, rx_buffer_data, RX_BUFFER_SIZE);
    sei();
}

uint8_t UART__getc(void) {
    uint8_t data;
    
    while (ring_buffer__pop(&rx_buffer, &data) == 0) {}
    
    return data;
}

void UART__putc(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))) {}
    
    UDR0 = data;
}

void UART__write(const uint8_t *buffer, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        UART__putc(buffer[i]);
    }
}

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    ring_buffer__push(&rx_buffer, data);
}

static int uart_putchar(char c, FILE *stream) {    
    if (c == '\n') {
        UART__putc('\r');
    }
    UART__putc(c);
    
    return 0;
}

static int uart_getchar(FILE *stream) {
    uint8_t c = UART__getc();
    
    if (c == '\r') {
        c = '\n';
    }
    
    return c;
}

static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void UART__init_stdio(void) {
    stdout = &uart_output;
    stderr = &uart_output;
    stdin = &uart_input;
}

void UART__read(uint8_t *buffer, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        buffer[i] = UART__getc();
    }
}

uint16_t UART__read_timeout(uint8_t *buffer, uint8_t n, uint16_t timeout_ms) {
    uint8_t read = 0;
    uint16_t counter;
    
    for (uint16_t i = 0; i < n; i++) {
        counter = 0;
        
        while (ring_buffer__pop(&rx_buffer, &buffer[i]) == 0) {
            for (uint16_t j = 0; j < 4000; j++) {
                __asm__ __volatile__("nop");
            }
            
            ++counter;
            if (counter >= timeout_ms) {
                return read;
            }
        }
        ++read;
    }
    
    return read;
}