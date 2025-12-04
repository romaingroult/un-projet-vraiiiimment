#include"buffer.h"

void initialize_buffer(struct buffer* buffer, uint8_t* data , uint8_t buffer_size){
    buffer->chars = data;
    buffer->read_id = 0;
    buffer->write_id = 0;
    buffer->length = 0;
    buffer->max_length = buffer_size;
}


uint8_t buffer_push(struct buffer* buffer, uint8_t data){
    if(buffer->length >= buffer->max_length){
        return 0;
    }
    buffer->chars[buffer->write_id] = data;
    ++buffer->write_id;
    ++buffer->length;
    if(buffer->write_id == buffer-> max_length)
        buffer->write_id = 0;
    return 1;
}


uint8_t buffer_pop(struct buffer* buffer, uint8_t * data){
    if(buffer->length > 0){
        *(data) = buffer->chars[buffer->read_id];
        ++buffer->read_id;
        --buffer->length;
        if(buffer->read_id == buffer->max_length)
            buffer->read_id = 0;
        return 1;
    }
    return 0;
}