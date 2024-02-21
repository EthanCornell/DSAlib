// ring_buffer.h
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>

// Structure for Copy-On-Write data handling
typedef struct {
    int data;
    atomic_int refCount;
} CoWData;

// RingBuffer structure definition
typedef struct {
    CoWData **buffer;
    atomic_int head, tail;
    int size;
    atomic_int count;
    pthread_mutex_t mutex; // Mutex for synchronization
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} RingBuffer;

// Function prototypes
RingBuffer* initialize(size_t size);
void insert(RingBuffer *rb, int value);
bool is_empty(RingBuffer *rb);
bool is_full(RingBuffer *rb);
CoWData* ringBufferRemove(RingBuffer *rb);
void modifyData(RingBuffer *rb, int index, int newValue);
void cleanupRingBuffer(RingBuffer *rb);

#endif // RING_BUFFER_H
