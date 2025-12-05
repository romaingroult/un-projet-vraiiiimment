#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define LED_DDR   DDRB
#define LED_PORT  PORTB
#define LED_PIN   PB5

void initialize_button(void);

uint8_t is_button_pressed(void);

#endif