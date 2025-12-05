#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "buffer.h"

static uint8_t buffer_data[BUFFER_SIZE];
static struct buffer buffer;

void UART__init(void) {
    uint16_t ubrr = MYUBRR;

    UCSR0A = (1 << U2X0);
    
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    initialize_buffer(&buffer, buffer_data, BUFFER_SIZE);
    sei();
}

uint8_t UART__getc(void) {
    uint8_t data;
    while (buffer_pop(&buffer, &data) == 0) {}
    return data;
}

void UART__putc(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))) {}
    UDR0 = data;
}

void UART__write(const uint8_t *source, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        UART__putc(source[i]);
    }
}

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    buffer_push(&buffer, data);
}

void UART__read(uint8_t *target, uint8_t *length) {
    uint8_t count = 0;
    while (buffer_pop(&buffer, &target[count])) {
        ++count;
        if (count >= 64) {
            break;
        }
    }
    *length = count;
}

uint16_t UART__available(void) {
    return buffer.length;
}
