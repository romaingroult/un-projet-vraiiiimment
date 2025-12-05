#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include "eeprom_registry.h"

#define PUBLIC_KEY_SIZE   40
#define SIGNATURE_SIZE    40

void initialize_crypto(void);

uint8_t generate_keypair(uint8_t *private_key, uint8_t *public_key);

uint8_t* generate_credential_id(void);

#endif