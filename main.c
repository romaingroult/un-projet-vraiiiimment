#include<string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "led.h"
#include "button.h"
#include "crypto.h"

#define STATUS_OK                     0
#define STATUS_ERR_COMMAND_UNKNOWN    1
#define STATUS_ERR_CRYPTO_FAILED      2
#define STATUS_ERR_BAD_PARAMETER      3
#define STATUS_ERR_NOT_FOUND          4
#define STATUS_ERR_STORAGE_FULL       5
#define STATUS_ERR_APPROVAL           6  

uint8_t waitForConsent(){
    uint8_t light = 0;
    for(uint8_t countdown = 0; countdown < 20 ;++countdown){
        if(light){
            led_off();
        }else led_on();
        
        light = !light;
        for (uint8_t j = 0; j < 50; j++) {
            _delay_ms(10);
            if (is_button_pressed()) {
                led_off();
                return 1;
            }
        }
    }
    return 0;
}

void makeCredentials(){
    const char * response = "makingCredentials";

    const uint8_t app_id[20];
    UART__read(app_id, 20);

    if(!waitForConsent()){
        UART__putc(STATUS_ERR_APPROVAL); 
        return; 
    }

    uint8_t private_key[PRIVATE_KEY_SIZE];
    uint8_t public_key[PUBLIC_KEY_SIZE];

    if(!generate_keypair(private_key, public_key)){
        UART__putc(STATUS_ERR_CRYPTO_FAILED);
        return;
    }

    uint8_t* credential_id = generate_credential_id();

    UART__putc(0x00);
    UART__write(credential_id, 16);
    UART__write(public_key, PUBLIC_KEY_SIZE);
}

void f2(){
    const char * response = "command 2";
    UART__write_chars(response);
}

void f3(){
    const char * response = "command 3";
    UART__write_chars(response);
}

void initialize(){
    UART__init();
    initialize_led();
    initialize_button();
    initialize_crypto();
}

int main(void) {

    initialize();

    const char * sentence = "type your command";
    //UART__write((const uint8_t*) sentence, strlen(sentence));
    
    while (1) {
        uint8_t command = UART__getc();
        
        switch (command) {
            case 0x01:
                makeCredentials();
                break;
                
            case '2':
                f2();
                break;
                
            case '3':
                f3();
                break;
                
            default:
                break;
        }
    }
    
    return 0;
}