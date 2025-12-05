#include "crypto.h"
#include "micro-ecc/uECC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

static uint32_t seed = 0x12345678;

static int rng_function(uint8_t *dest, unsigned size) {
    for (unsigned i = 0; i < size; i++) {
        ADCSRA |= _BV(ADSC);
        while (ADCSRA & _BV(ADSC));
        uint16_t adc_noise = ADC;
        
        uint8_t timer0_val = TCNT0;
        uint16_t timer1_val = TCNT1;
        uint8_t timer2_val = TCNT2;
        
        seed = seed * 1103515245UL + 12345UL;
        seed ^= adc_noise;
        seed ^= ((uint32_t)timer0_val << 16);
        seed ^= ((uint32_t)timer1_val << 8);
        seed ^= timer2_val;
        
        seed = (seed << 13) ^ seed;
        seed = (seed >> 17) ^ seed;
        seed = (seed << 5) ^ seed;
        
        dest[i] = (uint8_t)(seed >> 24);
        
        _delay_us(5);
    }
    
    return 1;
}

void initialize_crypto(void) {
    TCCR0B = _BV(CS01);
    TCCR1B = _BV(CS10);
    TCCR2B = _BV(CS22) | _BV(CS21);

    ADMUX = _BV(REFS0);
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);

    uint32_t entropy = 0;
    for (uint8_t i = 0; i < 32; i++) {
        ADCSRA |= _BV(ADSC);
        while (ADCSRA & _BV(ADSC));
        
        uint16_t adc = ADC;
        uint8_t t0 = TCNT0;
        uint16_t t1 = TCNT1;
        uint8_t t2 = TCNT2;
        
        entropy ^= adc;
        entropy ^= ((uint32_t)t0 << 16);
        entropy ^= ((uint32_t)t1 << 8);
        entropy ^= t2;
        entropy = (entropy << 1) | (entropy >> 31);
        
        _delay_us(100);
    }
    
    seed = entropy ? entropy : 0x5EED5EED;
    
    uECC_set_rng(&rng_function);
}

uint8_t generate_keypair(uint8_t *private_key, uint8_t *public_key) {
    int result = uECC_make_key(public_key, private_key, uECC_secp160r1());
    return (result == 1) ? 1 : 0;
}

void generate_credential_id(uint8_t *credential_id) {
    rng_function(credential_id, CREDENTIAL_ID_SIZE);
}

uint8_t sign_client_data(const uint8_t *private_key, const uint8_t *client_data, uint8_t *signature) {
    int result = uECC_sign(private_key, client_data, CLIENT_DATA_SIZE, signature, uECC_secp160r1());
    return (result == 1) ? 1 : 0;
}