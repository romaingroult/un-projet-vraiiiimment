#include<string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "led.h"
#include "button.h"
#include "crypto.h"
#include "eeprom_registry.h"

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
    const uint8_t app_id[APP_ID_SIZE];
    UART__read(app_id, APP_ID_SIZE);

    if(!can_register(app_id)){
        UART__putc(STATUS_ERR_STORAGE_FULL);
        return;
    }

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

    uint8_t credential_id[CREDENTIAL_ID_SIZE];
    generate_credential_id(credential_id);

    register_credential(app_id, credential_id, private_key);

    uint8_t response[57];
    response[0] = STATUS_OK;
    memcpy(&response[1], credential_id, CREDENTIAL_ID_SIZE);
    memcpy(&response[17], public_key, PUBLIC_KEY_SIZE);
    
    UART__write(response, 57);

}

void getAssertion(){
    uint8_t app_id[APP_ID_SIZE];
    uint8_t client_data[CLIENT_DATA_SIZE];

    UART__read(app_id, APP_ID_SIZE);
    UART__read(client_data, CLIENT_DATA_SIZE);

    uint8_t index = find_credential(app_id);
    if(index == MAX_CREDENTIALS){
        UART__putc(STATUS_ERR_NOT_FOUND);
        return;
    }

    if(!waitForConsent()){
        UART__putc(STATUS_ERR_APPROVAL); 
        return; 
    }

    uint8_t private_key[PRIVATE_KEY_SIZE];
    uint8_t credential_id[CREDENTIAL_ID_SIZE];

    get_credential(index, credential_id, private_key);

    uint8_t signature[CREDENTIAL_ID_SIZE];
    if(!sign_client_data(private_key, client_data, signature)){
        UART__putc(STATUS_ERR_CRYPTO_FAILED);
        return;
    }
    
    uint8_t response[57];
    response[0] = STATUS_OK;
    memcpy(&response[1], credential_id, CREDENTIAL_ID_SIZE);
    memcpy(&response[17], signature, SIGNATURE_SIZE);
    
    UART__write(response, 57);
}

void reinitializeRegistry(){
    if(!waitForConsent()){
        UART__putc(STATUS_ERR_APPROVAL); 
        return; 
    }
    reset_registry();
    UART__putc(STATUS_OK);
}

void listCredentials(){
    uint8_t count;
    uint8_t response[2 + MAX_CREDENTIALS*(APP_ID_SIZE + CREDENTIAL_ID_SIZE)];
    
    response[0] = STATUS_OK;
    list_credentials(&response[2], &count);
    response[1] = count;
    
    UART__write(response, 2 + count*(APP_ID_SIZE + CREDENTIAL_ID_SIZE));
}

void initialize(){
    UART__init();
    initialize_led();
    initialize_button();
    initialize_crypto();
    initialize_registry();
}

int main(void) {
    initialize();
    while (1) {
        uint8_t command = UART__getc();
        switch (command) {
            case 0x00:
                listCredentials();
                break;
            case 0x01:
                makeCredentials();
                break;
            case 0x02:
                getAssertion();
                break;
            case 0x03:
                reinitializeRegistry();
                break;
            default:
                break;
        }
    }
    
    return 0;
}