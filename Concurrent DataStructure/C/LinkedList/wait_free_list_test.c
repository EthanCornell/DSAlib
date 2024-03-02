#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h> // For assert()
#include "wait_free_list.h"

#define NUM_THREADS 10
#define OPERATIONS_PER_THREAD 100

WAIT_FREE_LockFreeList list;

void* thread_routine(void* arg) {
    int thread_id = *(int*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = thread_id * OPERATIONS_PER_THREAD + i;
        
        // Insert value into the list
        wait_free_list_insert(&(list.head), value);

        // Search the value in the list
        int found = wait_free_list_search(&(list.head), value);
        assert(found == 1); // Ensure that the inserted value is found

        // Delete the value from the list
        wait_free_list_delete(&(list.head), value);

        // Ensure the value is not found after deletion
        found = wait_free_list_search(&(list.head), value);
        assert(found == 0); // Ensure that the deleted value is not found
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    wait_free_list_init(&list);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_routine, &thread_ids[i]) != 0) {
            perror("Failed to create thread");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Verify the list is empty after all operations
    if (atomic_load(&list.head) != NULL) {
        printf("Test failed: list is not empty after all operations.\n");
    } else {
        printf("Test passed: list is empty after all operations.\n");
    }

    return EXIT_SUCCESS;
}


// Compile with: gcc -pg wait_free_list.c wait_free_list_test.c -o wait_free_list -pthread -O3
// Run with: ./wait_free_list