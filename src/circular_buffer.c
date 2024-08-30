#include "circular_buffer.h"
#include <stdlib.h>  // for malloc ve free
#include <string.h>  // for strlen için

void buffer_init(CircularBuffer *buf, size_t size) {
    buf->buffer = (char *)malloc(size);
    buf->size = size;
    buf->head = 0;
    buf->tail = 0;
    buf->count = 0;
}

void buffer_write(CircularBuffer *buf, const char *data) {
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++) {
        buf->buffer[buf->head] = data[i];
        buf->head = (buf->head + 1) % buf->size;
        if (buf->count == buf->size) {
            buf->tail = (buf->tail + 1) % buf->size;  // Write the oldest data if the buffer is full
        } else {
            buf->count++;
        }
    }
}

void buffer_read(CircularBuffer *buf, char *output, size_t len) {
    size_t i = 0;
    while (i < len && buf->count > 0) {
        output[i++] = buf->buffer[buf->tail];
        buf->tail = (buf->tail + 1) % buf->size;
        buf->count--;
    }
    output[i] = '\0';  // Add null terminator
}
