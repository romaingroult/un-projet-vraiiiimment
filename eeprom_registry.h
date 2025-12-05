#ifndef EEPROM_REGISTRY_H
#define EEPROM_REGISTRY_H

#include <stdint.h>

#define APP_ID_SIZE         20
#define CREDENTIAL_ID_SIZE  16
#define PRIVATE_KEY_SIZE    21

#define CREDENTIAL_SIZE     (APP_ID_SIZE + CREDENTIAL_ID_SIZE + PRIVATE_KEY_SIZE)

#define EEPROM_SIZE         1024
#define MAX_CREDENTIALS     (EEPROM_SIZE / CREDENTIAL_SIZE)

#define EMPTY_MARKER        0xFF


typedef struct {
    uint8_t app_id_hash[APP_ID_SIZE];
    uint8_t credential_id[CREDENTIAL_ID_SIZE];
    uint8_t private_key[PRIVATE_KEY_SIZE];
} credential;

void initialize_registry(void);

void reset_registry(void);

uint8_t register_credential(const uint8_t *app_id,const uint8_t *credential_id,const uint8_t *private_key);

uint8_t find_credential(const uint8_t *app_id);

uint8_t count_credentials(void);

uint8_t can_register(const uint8_t *app_id);

#endif