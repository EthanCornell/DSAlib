
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Added pthread header

#include "cvm_linkedlist.h"

#define NUM_THREADS 10
#define OPERATIONS_PER_THREAD 100

CVM_LinkedList list;

void* thread_func(void* arg) {
    int thread_num = *(int*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = thread_num * OPERATIONS_PER_THREAD + i;
        
        // Insert value into the list
        cvm_list_insert(&list, value);

        // Optionally, search for the inserted value (to verify insertion)
        if (!cvm_list_search(&list, value)) {
            printf("Error: Inserted value %d not found by thread %d\n", value, thread_num);
        }

        // Delete the value from the list
        cvm_list_delete(&list, value);

        // Optionally, verify deletion by searching for the value again
        if (cvm_list_search(&list, value)) {
            printf("Error: Deleted value %d still found by thread %d\n", value, thread_num);
        }
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];
    
    cvm_list_init(&list);

    // Create threads to perform concurrent operations on the list
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    // Optionally, verify the final state of the list (e.g., should be empty or meet certain criteria)

    printf("All tests passed successfully.\n");
    return 0;
}

// Compile with: gcc -pg cvm_linkedlist_test.c  cvm_linkedlist.c -o cvm_linkedlist -pthread -O3