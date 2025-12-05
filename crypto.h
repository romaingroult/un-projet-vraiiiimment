#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include "eeprom_registry.h"

#define PUBLIC_KEY_SIZE   40
#define SIGNATURE_SIZE    40
#define CLIENT_DATA_SIZE  20

void initialize_crypto(void);

uint8_t generate_keypair(uint8_t *private_key, uint8_t *public_key);

void generate_credential_id(uint8_t* credential_id);

uint8_t sign_client_data(const uint8_t *private_key, const uint8_t *client_data, uint8_t *signature);

#endif