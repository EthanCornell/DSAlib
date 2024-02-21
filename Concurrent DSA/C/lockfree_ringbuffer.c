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


#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h> // Include pthread header for mutex
#include "ring_buffer.h"


// Initialize the ring buffer
RingBuffer* initialize(size_t size) {
    RingBuffer *rb = (RingBuffer*)malloc(sizeof(RingBuffer));
    rb->buffer = (CoWData**)malloc(size * sizeof(CoWData*));
    rb->size = size;
    atomic_store(&rb->head, 0);
    atomic_store(&rb->tail, 0);
    atomic_store(&rb->count, 0);
    
    // Initialize condition variables
    pthread_cond_init(&rb->not_full, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
    pthread_mutex_init(&rb->mutex, NULL); // Initialize the mutex
    return rb;
}

// Insert data into the ring buffer
void insert(RingBuffer *rb, int value) {
    pthread_mutex_lock(&rb->mutex); // Acquire the lock to ensure exclusive access to the buffer.
    printf("Attempting to insert value: %d\n", value); // Log attempt to insert

    // Wait until there is space in the buffer.
    while ((atomic_load(&rb->head) + 1) % rb->size == atomic_load(&rb->tail)) {
        printf("Buffer full, waiting...\n"); // Log buffer full
        pthread_cond_wait(&rb->not_full, &rb->mutex);
    }

    // Insert the value into the buffer.
    int head = atomic_load(&rb->head);
    CoWData* newData = malloc(sizeof(CoWData)); // Allocate new data.
    if (!newData) {
        printf("Failed to allocate memory for new data\n"); // Log memory allocation failure
        pthread_mutex_unlock(&rb->mutex);
        return;
    }
    newData->data = value; // Set the value.
    atomic_store(&newData->refCount, 1); // Initialize the reference count.
    rb->buffer[head] = newData; // Place into the buffer.

    // Update the head to the next position.
    atomic_store(&rb->head, (head + 1) % rb->size);
    atomic_fetch_add(&rb->count, 1); // Increment the count of items in the buffer.

    printf("Inserted value: %d at position: %d\n", value, head); // Log successful insert

    pthread_cond_signal(&rb->not_empty); // Signal any waiting threads that the buffer is not empty.
    pthread_mutex_unlock(&rb->mutex); // Release the lock.
}


// Note: The actual implementation needs to handle memory management carefully,
// especially when removing items and managing the reference count.

// Function to check if the ring buffer is empty
bool is_empty(RingBuffer *rb) {
    int head = atomic_load_explicit(&rb->head, memory_order_acquire);
    int tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return head == tail;
}

// Function to check if the ring buffer is full
bool is_full(RingBuffer *rb) {
    int next_head = (atomic_load_explicit(&rb->head, memory_order_acquire) + 1) % rb->size;
    int tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return next_head == tail;
}

// Function to remove an item from the ring buffer, with added checks
CoWData* ringBufferRemove(RingBuffer *rb) {
    pthread_mutex_lock(&rb->mutex); // Acquire the lock to ensure exclusive access to the buffer.
    printf("Attempting to remove an item from the buffer...\n"); // Log attempt to remove

    // Wait until there is at least one item in the buffer.
    while (atomic_load(&rb->head) == atomic_load(&rb->tail)) {
        printf("Buffer empty, waiting...\n"); // Log buffer empty, waiting for items
        pthread_cond_wait(&rb->not_empty, &rb->mutex);
    }
    
    // Remove the value from the buffer.
    int tail = atomic_load(&rb->tail);
    CoWData* data = rb->buffer[tail]; // Retrieve the data to be removed.
    printf("Removing value: %d from position: %d\n", data->data, tail); // Log removal details
    
    rb->buffer[tail] = NULL; // Clear the buffer slot.
    
    // Update the tail to the next position.
    atomic_store(&rb->tail, (tail + 1) % rb->size);
    atomic_fetch_sub(&rb->count, 1); // Decrement the count of items in the buffer.
    printf("Item removed. New tail position: %d, Items in buffer now: %d\n", atomic_load(&rb->tail), atomic_load(&rb->count)); // Log new buffer state
    
    pthread_cond_signal(&rb->not_full); // Signal any waiting threads that the buffer is not full.
    pthread_mutex_unlock(&rb->mutex); // Release the lock.
    
    return data; // Return the removed data.
}





// Function to modify data with Copy-On-Write semantics
void modifyData(RingBuffer *rb, int index, int newValue) {
    // Calculate actual index considering the ring buffer's nature
    int actualIndex = (atomic_load_explicit(&rb->tail, memory_order_acquire) + index) % rb->size;
    
    pthread_mutex_lock(&rb->mutex); // Acquire lock before accessing rb->buffer
    CoWData* data = rb->buffer[actualIndex];
    if (data) { // Check if data is not NULL
        int refCount = atomic_load_explicit(&data->refCount, memory_order_acquire);
        if (refCount == 1) {
            // Safe to modify directly
            data->data = newValue;
        } else {
            // Else, handle CoW logic here. This might involve creating a new CoWData instance.
            // Not fully implemented due to complexity.
        }
    }
    pthread_mutex_unlock(&rb->mutex); // Release lock after accessing/modifying rb->buffer
}


//  Define a Thread Function for Concurrent Modification
// Simulate a Copy-On-Write operation by modifying the data
void* thread_modify(void* arg) {
    RingBuffer* rb = (RingBuffer*)arg;
    // Each thread attempts to modify a portion of the buffer
    
    for (int i = 0; i < rb->size; ++i) {
        int index = i % rb->size; // Wrap around to ensure we stay within bounds
        int newValue = i * 10; // Arbitrary new value for demonstration
        modifyData(rb, index, newValue);
    }
    return NULL;
}



// Cleanup function for RingBuffer
void cleanupRingBuffer(RingBuffer *rb) {
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->not_full);
    pthread_cond_destroy(&rb->not_empty);

    // Free memory allocated for CoWData and the buffer itself
    for (int i = 0; i < rb->size; ++i) {
        free(rb->buffer[i]);
    }
    free(rb->buffer);
    free(rb);
}


// Note: This implementation simplifies and omits some critical aspects of lock-free programming,
// such as safe memory reclamation (e.g., hazard pointers for managing `free` operations safely).
// compile: gcc -pg -o contest ./test/lockfree_rb_contest.c lockfree_ringbuffer.c -lpthread -O3
