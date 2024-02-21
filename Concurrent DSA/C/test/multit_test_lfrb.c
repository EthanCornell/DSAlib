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

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // Include stdlib.h for free() declaration
#include "../ring_buffer.h"

#define THREAD_COUNT 10
#define INSERTIONS_PER_THREAD 100
#define REMOVALS_PER_THREAD 100 // Assuming you want a balanced load

RingBuffer *rb;

// Thread function for inserting values
void* insert_values(void* arg) {
    int threadNum = *(int*)arg;
    for (int i = 0; i < INSERTIONS_PER_THREAD; i++) {
        insert(rb, threadNum * INSERTIONS_PER_THREAD + i);
    }
    return NULL;
}

// Thread function for removing values
void* remove_values(void* arg) {
    RingBuffer* rb = (RingBuffer*)arg;
    for (int i = 0; i < REMOVALS_PER_THREAD; i++) { // Ensure some exit condition in real scenarios
        CoWData* data = ringBufferRemove(rb);
        if (data) {
            printf("Removed value: %d\n", data->data);
            free(data);
        }
    }
    return NULL;
}

int main() {
    rb = initialize(THREAD_COUNT * INSERTIONS_PER_THREAD); // Adjust size as needed
    pthread_t insert_threads[THREAD_COUNT];
    pthread_t remove_threads[THREAD_COUNT];
    int threadNums[THREAD_COUNT];

    // Create threads to perform insertions
    for (int i = 0; i < THREAD_COUNT; i++) {
        threadNums[i] = i;
        pthread_create(&insert_threads[i], NULL, insert_values, &threadNums[i]);
    }

    // Create threads to perform removals
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&remove_threads[i], NULL, remove_values, rb);
    }

    // Wait for all insert threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(insert_threads[i], NULL);
    }

    // Wait for all remove threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(remove_threads[i], NULL);
    }

    // Cleanup
    cleanupRingBuffer(rb);

    printf("Multi-threaded operation test passed.\n");
    return 0;
}


// gcc -pg -o multitest multit_test_lfrb.c ../lockfree_ringbuffer.c -lpthread -O3