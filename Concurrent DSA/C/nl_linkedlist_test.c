#include <stdio.h>
#include <pthread.h> // Added pthread header
#include <assert.h> // Added assert header
#include "nl_linkedlist.h"

#define NUM_THREADS 10
#define OPERATIONS_PER_THREAD 100

NL_LinkedList list;

void* thread_func(void* arg) {
    int thread_num = *(int*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = thread_num * OPERATIONS_PER_THREAD + i;
        nl_list_insert(&list, value);

        // Verify that the insert worked
        assert(nl_list_search(&list, value) == 1);

        // Delete the value
        nl_list_delete(&list, value);

        // Verify that the delete worked
        assert(nl_list_search(&list, value) == 0);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];
    nl_list_init(&list);

    // Create threads to perform insertions and deletions
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

    // Final check to ensure list is empty
    assert(list.head == NULL);

    printf("All tests passed successfully.\n");
    return 0;
}


// gcc -pg nl_linkedlist.c nl_linkedlist_test.c -o nl_linkedlist -pthread -O3