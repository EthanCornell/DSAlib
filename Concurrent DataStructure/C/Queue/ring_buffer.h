/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */
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
