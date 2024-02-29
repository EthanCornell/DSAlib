#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "fgl_bst.h"

#define MAX_THREADS 128
#define OPERATIONS_PER_THREAD 1000

// Assume Node, Tree, insert, delete, and search are defined as before, with appropriate locking.

struct ThreadData {
    struct Tree* tree;
    int operation; // 0 for insert, 1 for delete, 2 for search
    int startValue; // Starting value for operations to ensure uniqueness where necessary
};

void* threadFunction(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        int value = data->startValue + i;
        switch (data->operation) {
            case 0: // Insert
                insert(data->tree, value);
                break;
            case 1: // Delete
                delete(data->tree, value);
                break;
            case 2: // Search
                search(data->tree, value);
                break;
        }
    }
    return NULL;
}

long long currentTimeMillis() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (long long)time.tv_sec * 1000 + time.tv_usec / 1000;
}

void performTest(struct Tree* tree, int operation, int numThreads) {
    pthread_t threads[MAX_THREADS];
    struct ThreadData threadData[MAX_THREADS];

    long long startTime = currentTimeMillis();

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].tree = tree;
        threadData[i].operation = operation;
        threadData[i].startValue = i * OPERATIONS_PER_THREAD;
        pthread_create(&threads[i], NULL, threadFunction, &threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    long long endTime = currentTimeMillis();
    long long duration = endTime - startTime;
    long long totalOperations = numThreads * OPERATIONS_PER_THREAD;
    printf("Threads: %d, Operation: %d, Duration (ms): %lld, Ops/ms: %f\n",
           numThreads, operation, duration, totalOperations / (double)duration);
}

int main() {
    struct Tree tree;
    // Initialize tree and mutex

    for (int threads = 1; threads <= MAX_THREADS; threads *= 2) {
        printf("Testing with %d threads:\n", threads);
        performTest(&tree, 0, threads); // Test insert
        performTest(&tree, 1, threads); // Test delete
        performTest(&tree, 2, threads); // Test search
    }

    // Cleanup tree and mutexes
    return 0;
}

// gcc -pg fgl_bst_test.c fgl_bst.c -o fgl_bst -pthread -O3