#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main(void) {
    UART__init();
    
    UART__putc('O');
    UART__putc('K');
    UART__putc('\r');
    UART__putc('\n');
    
    while (1) {
        uint8_t c = UART__getc();
        UART__putc(c);
    }
    
    return 0;
}
