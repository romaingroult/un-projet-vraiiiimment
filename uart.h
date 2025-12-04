#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>

#define F_CPU 16000000UL

#define BAUD 115200
#define MYUBRR ((F_CPU / 8 / BAUD) - 1)

#define BUFFER_SIZE 64

void UART__init(void);
void UART__init_stdio(void);

uint8_t UART__getc(void);

void UART__putc(uint8_t data);
void UART__write(const uint8_t *source, uint16_t n);
void UART__write_chars(const char *source);

void UART__read(uint8_t *target, uint16_t n);
uint16_t UART__read_timeout(uint8_t *target, uint16_t n, uint16_t timeout_ms);

#endif