#include <avr/io.h>

#define LED_DDR   DDRB
#define LED_PORT  PORTB
#define LED_PIN   PB5

#define CONSENT_TIMEOUT_SEC  10

void initialize_led(void){
    LED_DDR |= (1 << LED_PIN);
    LED_PORT &= ~(1 << LED_PIN);
}

void led_on(void){
    PORTD |= _BV(PD4);
}

void led_off(void){
    PORTD &= ~_BV(PD4);
}