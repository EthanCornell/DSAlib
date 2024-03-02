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
#include <pthread.h>
#include <assert.h>
#include "../ring_buffer.h" 

#define PRODUCER_THREAD_COUNT 5
#define CONSUMER_THREAD_COUNT 5
#define OPERATIONS_PER_THREAD 100

RingBuffer *rb;

// Producer thread function
void* producer(void* arg) {
    int threadNum = *(int*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = threadNum * OPERATIONS_PER_THREAD + i;
        insert(rb, value);
        printf("Producer %d inserted value: %d\n", threadNum, value);
    }
    return NULL;
}

// Consumer thread function
void* consumer(void* arg) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        CoWData* data = ringBufferRemove(rb);
        if (data) {
            printf("Consumer removed value: %d\n", data->data);
            free(data); // Ensure you handle memory deallocation properly
        }
    }
    return NULL;
}

int main() {
    rb = initialize(PRODUCER_THREAD_COUNT * OPERATIONS_PER_THREAD); // Initialize the ring buffer with adequate size
    pthread_t producers[PRODUCER_THREAD_COUNT];
    pthread_t consumers[CONSUMER_THREAD_COUNT];
    int threadNums[PRODUCER_THREAD_COUNT]; // Used for passing thread numbers to producers

    // Start producer threads
    for (int i = 0; i < PRODUCER_THREAD_COUNT; i++) {
        threadNums[i] = i;
        if (pthread_create(&producers[i], NULL, producer, &threadNums[i])) {
            perror("Failed to create producer thread");
            exit(EXIT_FAILURE);
        }
    }

    // Start consumer threads
    for (int i = 0; i < CONSUMER_THREAD_COUNT; i++) {
        if (pthread_create(&consumers[i], NULL, consumer, NULL)) {
            perror("Failed to create consumer thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all producer threads to finish
    for (int i = 0; i < PRODUCER_THREAD_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }

    // Wait for all consumer threads to finish
    for (int i = 0; i < CONSUMER_THREAD_COUNT; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Cleanup
    cleanupRingBuffer(rb);

    printf("Concurrent test completed.\n");
    return 0;
}

// gcc -pg -o contest con_test_lfrb.c ../lockfree_ringbuffer.c -lpthread -O3