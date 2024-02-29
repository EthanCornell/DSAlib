#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // For uintptr_t
#include <pthread.h>
#include <time.h>
#include "fgl_linkedlist.h"
#include "lock_free_list.h"
#include "cvm_linkedlist.h"
#include "wait_free_list.h"
#include "nl_linkedlist.h"

#define MAX_THREADS 5 // Adjust based on your system's capabilities
#define OPERATIONS_PER_THREAD 10000

NL_LinkedList nl_list;
FGL_Node* fgl_head;
LockFreeList lock_free_list;
CVM_LinkedList cvm_list;
WAIT_FREE_LockFreeList wait_free_list;

pthread_mutex_t mutex; // Correct declaration of a mutex

typedef enum {
    DS_NAIVE_LOCKING,
    DS_FINE_GRAINED_LOCKING,
    DS_LOCK_FREE,
    DS_CV_MUTEXES,
    DS_WAIT_FREE
} DataStructureType;

// Function prototypes
void nl_list_cleanup(NL_LinkedList* list);
void fgl_list_cleanup(FGL_Node** head);
void lock_free_list_cleanup(LockFreeList* list);
void cvm_list_cleanup(CVM_LinkedList* list);
void wait_free_list_cleanup(WAIT_FREE_LockFreeList* list);

void data_structure_init(DataStructureType current_ds);
void data_structure_insert(int data, DataStructureType current_ds);
void data_structure_delete(int data, DataStructureType current_ds);
int data_structure_search(int data, DataStructureType current_ds);
void* thread_operation(void* arg);

// DataStructureType current_ds = DS_FINE_GRAINED_LOCKING; // Default, can be changed based on input or testing needs
void data_structure_init(DataStructureType current_ds) {
    printf("Initializing data structure: %d\n", current_ds);
    // Initialize based on the current data structure
    switch (current_ds) {
        case DS_NAIVE_LOCKING:
            nl_list_init(&nl_list);
            break;
        case DS_FINE_GRAINED_LOCKING:
            fgl_list_init(&fgl_head);
            break;
        case DS_LOCK_FREE:
            Lock_Free_list_init(&lock_free_list);
            break;
        case DS_CV_MUTEXES:
            cvm_list_init(&cvm_list);
            break;
        case DS_WAIT_FREE:
            wait_free_list_init(&wait_free_list);
            break;
    }
}

void data_structure_insert(int data, DataStructureType current_ds) {
    // printf("Inserting data: %d into DS: %d\n", data, current_ds);
    // Insert based on the current data structure
    switch (current_ds) {
        case DS_NAIVE_LOCKING:
            nl_list_insert(&nl_list, data);
            break;
        case DS_FINE_GRAINED_LOCKING:
            fgl_list_insert(&fgl_head, data);
            break;
        case DS_LOCK_FREE:
            Lock_Free_list_insert(&lock_free_list, data);
            break;
        case DS_CV_MUTEXES:
            cvm_list_insert(&cvm_list, data);
            break;
        case DS_WAIT_FREE:
            wait_free_list_insert(&wait_free_list.head, data);
            break;
    }
}

void data_structure_delete(int data, DataStructureType current_ds) {
    // printf("Deleting data: %d from DS: %d\n", data, current_ds);
    // Delete based on the current data structure
    switch (current_ds) {
        case DS_NAIVE_LOCKING:
            nl_list_delete(&nl_list, data);
            break;
        case DS_FINE_GRAINED_LOCKING:
            fgl_list_delete(&fgl_head, data);
            break;
        case DS_LOCK_FREE:
            Lock_Free_list_delete(&lock_free_list, data);
            break;
        case DS_CV_MUTEXES:
            cvm_list_delete(&cvm_list, data);
            break;
        case DS_WAIT_FREE:
            wait_free_list_delete(&wait_free_list.head, data);
            break;
    }
}

int data_structure_search(int data, DataStructureType current_ds) {
    // printf("Searching data: %d in DS: %d\n", data, current_ds);
    // Search based on the current data structure
    int found = 0;
    switch (current_ds) {
        case DS_NAIVE_LOCKING:
            found = nl_list_search(&nl_list, data);
            break;
        case DS_FINE_GRAINED_LOCKING:
            found = fgl_list_search(fgl_head, data);
            break;
        case DS_LOCK_FREE:
            found = Lock_Free_list_search(&lock_free_list, data);
            break;
        case DS_CV_MUTEXES:
            found = cvm_list_search(&cvm_list, data);
            break;
        case DS_WAIT_FREE:
            found = wait_free_list_search(&wait_free_list.head, data);
            break;
    }
    return found;
}


// Cleanup for Naive Locking List
void nl_list_cleanup(NL_LinkedList* list) {
    NL_Node* current = list->head;
    while (current != NULL) {
        NL_Node* temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
}

// Cleanup for Fine-Grained Locking List
void fgl_list_cleanup(FGL_Node** head) {
    FGL_Node* current = *head;
    while (current != NULL) {
        FGL_Node* temp = current;
        current = current->next;
        pthread_mutex_destroy(&temp->lock);
        free(temp);
    }
    *head = NULL;
}

// Add similar cleanup functions for LockFreeList, CVM_LinkedList, and WAIT_FREE_LockFreeList
// For simplicity, pseudocode is provided. Actual implementation will depend on the specific data structure.

#include <stdatomic.h>
#include <stdlib.h>

void lock_free_list_cleanup(LockFreeList* list) {
    // Correctly typed pointer for atomic_load result
    atomic_uintptr_t atomic_head;
    LOCK_FREE_Node* head;
    LOCK_FREE_Node* next;

    // Load the current head of the list atomically and cast appropriately
    while ((atomic_head = atomic_load(&list->head)) != 0) {
        head = (LOCK_FREE_Node*)atomic_head; // Cast atomic_head to LOCK_FREE_Node pointer
        // Ensure next is loaded correctly, using atomic operation and cast
        next = (LOCK_FREE_Node*)atomic_load(&(head->next));
        
        // Free the current head node
        free(head);
        
        // Attempt to atomically update the head to the next node
        // Direct atomic_store might be more appropriate here, as mentioned previously
        atomic_store(&list->head, (atomic_uintptr_t)next);
    }
}



void cvm_list_cleanup(CVM_LinkedList* list) {
    CVM_Node* current = list->head;
    while (current != NULL) {
        CVM_Node* temp = current;
        current = current->next;
        free(temp); // Free the node
    }
    list->head = NULL; // Reset head to NULL
    pthread_mutex_destroy(&list->lock); // Destroy the mutex
    pthread_cond_destroy(&list->cond); // Destroy the condition variable
}


void wait_free_list_cleanup(WAIT_FREE_LockFreeList* list) {
    while (atomic_load(&list->head) != NULL) {
        WAIT_FREE_Node* head = atomic_load(&list->head);
        WAIT_FREE_Node* next = atomic_load(&head->next);
        free(head); // Free the node
        atomic_compare_exchange_weak(&list->head, &head, next); // Attempt to update head to next node
    }
}


// Call cleanup at the end of main or before reinitializing a data structure
// Thread operation function
void* thread_operation(void* arg) {
    DataStructureType ds = *((DataStructureType*)arg);
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int data = rand(); // Use random data for each operation
        data_structure_insert(data, ds);
        if (!data_structure_search(data, ds)) {
            printf("Error: Data %d not found\n", data);
            exit(EXIT_FAILURE);
        }
        data_structure_delete(data, ds);
        if (data_structure_search(data, ds)) {
            printf("Error: Deleted data %d still found\n", data);
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

// Main benchmarking function
int main() {
    // Correct initialization of the mutex
    
    for (int ds = DS_CV_MUTEXES; ds <= DS_WAIT_FREE; ds++) {
        pthread_mutex_init(&mutex, NULL);
        printf("\n--- Testing Data Structure: %d ---\n", ds);
        for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2) {
            // Initialize data structure
            data_structure_init(ds);
            
            pthread_t threads[thread_count];
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            // Create and start threads
            for (int i = 0; i < thread_count; i++) {
                
                if (pthread_create(&threads[i], NULL, thread_operation, &ds)) {
                    perror("Failed to create thread");
                    return EXIT_FAILURE;
                }
            }

            // Wait for all threads to complete
            for (int i = 0; i < thread_count; i++) {
                if (pthread_join(threads[i], NULL) != 0) {
                    perror("Failed to join thread");
                    return EXIT_FAILURE;
                }
            }

            // End timing
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calculate and print throughput
            double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
            double throughput = (thread_count * OPERATIONS_PER_THREAD * 3) / elapsed_time; // *3 for insert, search, delete
            // printf("Data Structure: %d, Threads: %d, Throughput: %f ops/ms\n", ds, thread_count, throughput);
            printf("Threads: %d, Throughput: %f ops/ms\n", thread_count, throughput);
        }
        printf("\n"); // New line for readability between data structure tests
        nl_list_cleanup(&nl_list);
        fgl_list_cleanup(&fgl_head);
        lock_free_list_cleanup(&lock_free_list);
        cvm_list_cleanup(&cvm_list);
        wait_free_list_cleanup(&wait_free_list);
    }

    // nl_list_cleanup(&nl_list);
    // fgl_list_cleanup(&fgl_head);
    // lock_free_list_cleanup(&lock_free_list);
    // cvm_list_cleanup(&cvm_list);
    // wait_free_list_cleanup(&wait_free_list);

    // Destroy the mutex before exiting
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}


// gcc -pg -o benchmark benchmark.c fgl_linkedlist.c lock_free_list.c cvm_linkedlist.c wait_free_list.c nl_linkedlist.c -pthread -O3
