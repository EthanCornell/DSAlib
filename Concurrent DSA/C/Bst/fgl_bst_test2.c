#include "fgl_bst.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> // Include this for gettimeofday


#define NUM_THREADS 32
#define OPERATIONS_PER_THREAD 10000

struct ThreadData {
    struct Tree* tree;
    int threadId;
    long long startTime; // Start time in milliseconds
    long long endTime;   // End time in milliseconds
};


void* threadInsert(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    struct timeval start, end;

    printf("Thread %d (Insert): Starting\n", data->threadId);
    gettimeofday(&start, NULL); // Get start time
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        insert(data->tree, data->threadId * OPERATIONS_PER_THREAD + i);
    }
    gettimeofday(&end, NULL); // Get end time
    printf("Thread %d (Insert): Completed\n", data->threadId);

    // Convert and store start and end times in milliseconds
    data->startTime = (long long)start.tv_sec * 1000 + start.tv_usec / 1000;
    data->endTime = (long long)end.tv_sec * 1000 + end.tv_usec / 1000;

    return NULL;
}

void* threadDelete(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    struct timeval start, end;

    gettimeofday(&start, NULL); // Get start time
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        delete(data->tree, data->threadId * OPERATIONS_PER_THREAD + i);
    }
    gettimeofday(&end, NULL); // Get end time

    // Convert and store start and end times in milliseconds
    data->startTime = (long long)start.tv_sec * 1000 + start.tv_usec / 1000;
    data->endTime = (long long)end.tv_sec * 1000 + end.tv_usec / 1000;

    return NULL;
}


void* threadSearch(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    struct timeval start, end;

    gettimeofday(&start, NULL); // Get start time
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        search(data->tree, data->threadId * OPERATIONS_PER_THREAD + i);
    }
    gettimeofday(&end, NULL); // Get end time

    // Convert and store start and end times in milliseconds
    data->startTime = (long long)start.tv_sec * 1000 + start.tv_usec / 1000;
    data->endTime = (long long)end.tv_sec * 1000 + end.tv_usec / 1000;

    return NULL;
}


int main() {
    struct Tree tree;
    pthread_mutex_init(&tree.mutex, NULL);
    tree.root = NULL;

    pthread_t threads[NUM_THREADS];
    struct ThreadData threadData[NUM_THREADS];

    // Initialize thread data
    for (int i = 0; i < NUM_THREADS; ++i) {
        threadData[i].tree = &tree;
        threadData[i].threadId = i;
    }

    // Launch threads for insert and delete operations
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (i < NUM_THREADS / 2) {
            pthread_create(&threads[i], NULL, threadInsert, &threadData[i]);
        } else {
            pthread_create(&threads[i], NULL, threadDelete, &threadData[i]);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Compute and print operations per millisecond for each thread
    printf("Performance Results:\n");
    for (int i = 0; i < NUM_THREADS; ++i) {
        long long duration = threadData[i].endTime - threadData[i].startTime;
        if (duration == 0) duration = 1; // Prevent division by zero
        double opsPerMs = (double)OPERATIONS_PER_THREAD / duration;
        printf("Thread %d: Ops/ms = %f\n", i, opsPerMs);
    }

    printf("Test completed.\n");

    // Cleanup
    pthread_mutex_destroy(&tree.mutex);
    // Note: Properly free all nodes in the tree to avoid memory leaks

    return 0;
}

// gcc -pg fgl_bst_test2.c fgl_bst.c -o fgl_bst -pthread -O3