#include <avr/io.h>

void led_on(void){
    PORTD |= _BV(PD4);
}

void led_off(void){
    PORTD &= ~_BV(PD4);
}