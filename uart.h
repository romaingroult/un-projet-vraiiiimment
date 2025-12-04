#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>

#define F_CPU 16000000UL

#define BAUD 9600
#define MYUBRR ((F_CPU / 8 / BAUD) - 1)

#define RX_BUFFER_SIZE 64

void UART__init(void);
void UART__init_stdio(void);

uint8_t UART__getc(void);

void UART__putc(uint8_t data);
void UART__write(const uint8_t *buffer, uint16_t n);

void UART__read(uint8_t *buffer, uint16_t n);
uint16_t UART__read_timeout(uint8_t *buffer, uint8_t n, uint16_t timeout_ms);

#endif