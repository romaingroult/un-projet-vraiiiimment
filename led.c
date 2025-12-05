#include <avr/io.h>

void initialize_led(void) {
    DDRD |= _BV(PD4);
    PORTD &= ~_BV(PD4);
}

void led_on(void){
    PORTD |= _BV(PD4);
}

void led_off(void){
    PORTD &= ~_BV(PD4);
}