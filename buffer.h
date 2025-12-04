#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

struct buffer {
    uint8_t * chars;
    uint8_t read_id;
    uint8_t write_id;
    uint8_t length;
    uint8_t max_length;
};

void initialize_buffer(struct buffer* buffer, uint8_t* data, uint8_t buffer_size);

uint8_t buffer_push(struct buffer* buffer, uint8_t data);

uint8_t buffer_pop(struct buffer* buffer, uint8_t * data);

#endif
