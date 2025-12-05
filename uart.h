#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>

#define F_CPU 16000000UL

#define BAUD 115200
#define MYUBRR ((F_CPU / 8 / BAUD) - 1)

#define BUFFER_SIZE 64

void UART__init(void);

uint8_t UART__getc(void);
void UART__putc(uint8_t data);

void UART__write(const uint8_t *source, uint16_t n);
void UART__read(uint8_t *target, uint8_t* length);

uint16_t UART__available(void);

#endif