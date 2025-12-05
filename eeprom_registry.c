#include "eeprom_registry.h"
#include <avr/eeprom.h>
#include <string.h>

#define EEPROM_BASE_ADDR 0

#define MAGIC_VALUE_1      0xCA
#define MAGIC_VALUE_2      0xFE


static uint16_t calculate_address(uint8_t index) {
    return EEPROM_BASE_ADDR + (index * CREDENTIAL_SIZE);
}

static uint8_t is_already_initialized(void) {
    uint8_t address = calculate_address(MAX_CREDENTIALS);
    uint8_t magic1 = eeprom_read_byte((uint8_t*)address);
    uint8_t magic2 = eeprom_read_byte((uint8_t*)(address + 1));
    
    return magic1 == MAGIC_VALUE_1 && magic2 == MAGIC_VALUE_2;
}


void reset_registry(void){
    for (uint8_t slot = 0;  slot < MAX_CREDENTIALS; slot++) {
        int16_t address = calculate_address(slot);
        eeprom_write_byte((uint8_t*)address, EMPTY_MARKER);
    }
}

void initialize_registry(void) {
    if (is_already_initialized()) {
        return;
    }

    reset_registry();

    uint8_t address = calculate_address(MAX_CREDENTIALS);
    eeprom_write_byte((uint8_t*)address, MAGIC_VALUE_1);
    eeprom_write_byte((uint8_t*)(address + 1), MAGIC_VALUE_2);
}

static uint8_t is_slot_empty(uint8_t index) {
    uint16_t address = calculate_address(index);
    return (eeprom_read_byte((uint8_t*)address) == EMPTY_MARKER);
}

static uint8_t hash_equals(const uint8_t *app_id, const uint8_t *app_id2) {
    for (uint8_t i = 0; i < APP_ID_SIZE; i++) {
        if (app_id[i] != app_id2[i]) {
            return 0;
        }
    }
    return 1;
}

static uint8_t find_empty_slot(){
    for (uint8_t index = 0; index < MAX_CREDENTIALS; index++) {
        if (is_slot_empty(index)) {
            return index;
        }
    }
    return MAX_CREDENTIALS;
}


uint8_t register_credential(const uint8_t *app_id, const uint8_t *credential_id, const uint8_t *private_key){
    uint8_t index = find_credential(app_id);
    if(index != MAX_CREDENTIALS){  
        uint16_t address = calculate_address(index);
        eeprom_write_block(credential_id, (void*)(address + APP_ID_SIZE), CREDENTIAL_ID_SIZE);
        eeprom_write_block(private_key, (void*)(address + APP_ID_SIZE + CREDENTIAL_ID_SIZE), PRIVATE_KEY_SIZE);
        return 1;
    }
    index = find_empty_slot();
    if(index != MAX_CREDENTIALS){  
        uint16_t address = calculate_address(index);
        eeprom_write_block(app_id, (void*)address, APP_ID_SIZE);
        eeprom_write_block(credential_id, (void*)(address + APP_ID_SIZE), CREDENTIAL_ID_SIZE);
        eeprom_write_block(private_key, (void*)(address + APP_ID_SIZE + CREDENTIAL_ID_SIZE), PRIVATE_KEY_SIZE);
        return 1;
    }
    return 0;
}

uint8_t find_credential(const uint8_t* app_id) {
    uint8_t temp_app_id[APP_ID_SIZE];
    
    for (uint8_t index = 0; index < MAX_CREDENTIALS; index++) {
        if (is_slot_empty(index)) {
            continue;
        }
        
        uint16_t address = calculate_address(index);
        eeprom_read_block(temp_app_id, (void*)address, APP_ID_SIZE);
        
        if (hash_equals(temp_app_id, app_id)) {
            return index;
        }
    }
    
    return MAX_CREDENTIALS;
}

uint8_t count_credentials(void) {
    uint8_t count = 0;
    
    for (uint8_t index = 0; index < MAX_CREDENTIALS; index++) {
        if (!is_slot_empty(index)) {
            count++;
        }
    }
    
    return count;
}

uint8_t can_register(const uint8_t *app_id){
    return find_credential(app_id) != MAX_CREDENTIALS || find_empty_slot() != MAX_CREDENTIALS;
}


void get_credential(uint8_t index, uint8_t* credential_id, uint8_t* private_key){
    int16_t address = calculate_address(index);
    eeprom_read_block(credential_id, (void*) (address + APP_ID_SIZE), CREDENTIAL_ID_SIZE);
    eeprom_read_block(private_key, (void*) (address + APP_ID_SIZE + CREDENTIAL_ID_SIZE), PRIVATE_KEY_SIZE);
}

void list_credentials(uint8_t *list, uint8_t *count) {
    uint8_t current = 0;
    for (uint8_t index = 0; index < MAX_CREDENTIALS; ++index) {
        if (!is_slot_empty(index)) {
            uint16_t address = calculate_address(index);
            eeprom_read_block(&list[current * 36], (void*)(address + APP_ID_SIZE), CREDENTIAL_ID_SIZE);
            eeprom_read_block(&list[current * 36 + CREDENTIAL_ID_SIZE],(void*)address, APP_ID_SIZE);
            ++current;
        }
    }
    *count = current;
}