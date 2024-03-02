// fgl_linkedlist_test
// // gcc -pg fgl_linkedlist_test.c fgl_linkedlist.c -o fgl_linkedlist -pthread -O3




#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "fgl_linkedlist.h"

#define MAX_THREADS 128
#define OPERATIONS_PER_THREAD 100

FGL_Node* head = NULL;

// Thread function and other implementations remain the same

void* deadlock_test_thread_func(void* arg) {
    // Intentionally create a scenario that might lead to a deadlock
    // For example, lock two nodes in the opposite order in different threads

    // This is a simplistic approach and may need to be adjusted based on your list's structure and operations
    int first_op = *(int*)arg;
    int second_op = first_op == 0 ? 1 : 0; // Simple switch between two operations for demonstration

    if (first_op == 0) {
        // Thread 1: insert then delete
        fgl_list_insert(&head, 999); // Value chosen for test; ensure it's unique
        fgl_list_delete(&head, 999);
    } else {
        // Thread 2: delete then insert, attempting to reverse the operation order
        fgl_list_delete(&head, 999);
        fgl_list_insert(&head, 999);
    }

    return NULL;
}

void run_deadlock_test() {
    pthread_t test_threads[2];
    int ops[2] = {0, 1}; // Two operations representing different actions

    for (int i = 0; i < 2; i++) {
        if (pthread_create(&test_threads[i], NULL, deadlock_test_thread_func, &ops[i]) != 0) {
            perror("Failed to create deadlock test thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 2; i++) {
        if (pthread_join(test_threads[i], NULL) != 0) {
            perror("Failed to join deadlock test thread");
            exit(EXIT_FAILURE);
        }
    }
}



void* thread_func(void* arg) {
    int thread_num = *(int*)arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = thread_num * OPERATIONS_PER_THREAD + i;
        fgl_list_insert(&head, value);

        int found = fgl_list_search(head, value);
        if (!found) {
            printf("Error: Inserted value %d not found\n", value);
            exit(EXIT_FAILURE);
        }

        fgl_list_delete(&head, value);

        found = fgl_list_search(head, value);
        if (found) {
            printf("Error: Deleted value %d still found\n", value);
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}


void run_concurrent_test(int num_threads) {
    pthread_t threads[MAX_THREADS];
    int thread_nums[MAX_THREADS];
    struct timespec start, end;

    // Initialize the list
    head = NULL;

    // Record start time
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < num_threads; i++) {
        thread_nums[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }

    // Record end time
    clock_gettime(CLOCK_REALTIME, &end);

    // Calculate and print the execution time
    double elapsed_time = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6; // milliseconds
    printf("Execution time with %d threads: %.2f milliseconds\n", num_threads, elapsed_time);

    // Calculate throughput in operations per millisecond
    int total_operations = num_threads * OPERATIONS_PER_THREAD * 2; // Insert and delete operations
    double throughput = total_operations / elapsed_time; // operations per millisecond
    printf("Throughput with %d threads: %.2f operations per millisecond\n", num_threads, throughput);
}

int main() {
    printf("Running deadlock test...\n");
    run_deadlock_test();
    printf("Deadlock test completed.\n");

    // Run concurrent tests for varying number of threads
    for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads++) {
        run_concurrent_test(num_threads);
    }

    printf("All tests passed successfully.\n");
    return 0;
}


// int main() {
//     // Previous setup and thread creation code remains the same

//     // After completing the main test, run the deadlock test
//     printf("Running deadlock test...\n");
//     run_deadlock_test();

//     // Continue with the rest of the main function
//     printf("Deadlock test completed.\n");


//     // Concurrent test
//     printf("Running Concurrent test...\n");
//     pthread_t threads[NUM_THREADS];
//     int thread_nums[NUM_THREADS];
//     struct timespec start, end;

//     // Initialize the list
//     head = NULL;

//     // Record start time
//     clock_gettime(CLOCK_REALTIME, &start);

//     for (int i = 0; i < NUM_THREADS; i++) {
//         thread_nums[i] = i;
//         if (pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]) != 0) {
//             perror("Failed to create thread");
//             return 1;
//         }
//     }

//     for (int i = 0; i < NUM_THREADS; i++) {
//         if (pthread_join(threads[i], NULL) != 0) {
//             perror("Failed to join thread");
//             return 1;
//         }
//     }

//     // Record end time
//     clock_gettime(CLOCK_REALTIME, &end);

//     // Calculate and print the execution time
//     double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     printf("Execution time: %.2f seconds\n", elapsed_time);

//     // Calculate throughput
//     int total_operations = NUM_THREADS * OPERATIONS_PER_THREAD * 2; // Insert and delete operations
//     double throughput = total_operations / elapsed_time;
//     printf("Throughput: %.2f operations per second\n", throughput);

//     printf("All tests passed successfully.\n");
//     return 0;

// }


// Remember to compile with the -pthread flag and time measurement:
// gcc -pg fgl_linkedlist_test.c fgl_linkedlist.c -o fgl_linkedlist -pthread -O3
