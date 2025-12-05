#ifndef LED_H
#define LED_H

#define LED_DDR   DDRB
#define LED_PORT  PORTB
#define LED_PIN   PB5

void initialize_led(void); 

void led_on(void);

void led_off(void);

#endif