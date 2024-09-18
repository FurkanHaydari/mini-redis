#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h> // For size_t definition

typedef struct
{
    char *buffer;
    size_t size;
    size_t head;
    size_t tail;
    size_t count;
} CircularBuffer;

void buffer_init(CircularBuffer *buf, size_t size);
void buffer_write(CircularBuffer *buf, const char *data);
void buffer_read(CircularBuffer *buf, char *output, size_t len);

#endif // CIRCULAR_BUFFER_H
