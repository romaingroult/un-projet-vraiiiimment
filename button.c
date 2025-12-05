#include <stdint.h>
#include <avr/io.h>

#define BUTTON_DDR   DDRD
#define BUTTON_PORT  PORTD
#define BUTTON_PIN   PIND
#define BUTTON_BIT   PD2


void initialize_button(void){
    BUTTON_DDR &= ~(1 << BUTTON_BIT);
    BUTTON_PORT |= (1 << BUTTON_BIT);
}

uint8_t is_button_pressed(void){
    return !(BUTTON_PIN & (1 << BUTTON_BIT));
}
