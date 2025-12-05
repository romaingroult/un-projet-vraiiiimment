#include "crypto.h"
#include "micro-ecc/uECC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

static uint32_t seed = 0x12345678;

static int rng_function(uint8_t *dest, unsigned size) {
    static uint8_t adc_initialized = 0;
    if (!adc_initialized) {
        ADMUX = (1 << REFS0);
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
        adc_initialized = 1;
    }
    
    for (unsigned i = 0; i < size; i++) {
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        uint16_t adc_noise = ADC;

        seed = seed * 1103515245 + 12345 + adc_noise;
        seed ^= (TCNT0 << 8) | TCNT1;
        
        dest[i] = (uint8_t)(seed >> 24);
    }
    
    return 1;
}

void initialize_crypto(void) {
    TCCR0B = (1 << CS01);
    
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
    
    for (uint8_t i = 0; i < 10; i++) {
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        seed ^= ADC;
        seed = (seed << 1) | (seed >> 31);
        _delay_us(100);
    }
    uECC_set_rng(&rng_function);
}

uint8_t generate_keypair(uint8_t *private_key, uint8_t *public_key) {
    int result = uECC_make_key(public_key, private_key, uECC_secp160r1());
    return (result == 1) ? 1 : 0;
}

uint8_t* generate_credential_id() {
    uint8_t credential_id[16];
    rng_function(credential_id, 16);
    return credential_id;
}