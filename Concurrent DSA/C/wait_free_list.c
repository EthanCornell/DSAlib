
// wait-free linked list
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
// #include <pthread.h>
#include "wait_free_list.h"

// #define MARKED_PTR(p)   (Node*)((uintptr_t)(p) | 1)
// #define UNMARKED_PTR(p) (Node*)((uintptr_t)(p) & ~1)
// #define IS_MARKED(p)    ((uintptr_t)(p) & 1)

// typedef struct Node {
//     int data;
//     _Atomic(struct Node*) next; // Correctly define next as an atomic pointer to Node
// } Node;

// // Define LockFreeList correctly
// typedef struct LockFreeList {
//     _Atomic(Node*) head; // Correctly define head as an atomic pointer to Node
// } LockFreeList;


void wait_free_list_init(WAIT_FREE_LockFreeList* list) {
    atomic_store(&list->head, NULL); // Correct initialization
}

WAIT_FREE_Node* wait_free_create_node(int data) {
    WAIT_FREE_Node* node = (WAIT_FREE_Node*)malloc(sizeof(WAIT_FREE_Node));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    atomic_store(&node->next, NULL); // Correctly initialize the atomic next pointer
    return node;
}

void wait_free_list_insert(_Atomic(WAIT_FREE_Node*)* head, int data) {
    WAIT_FREE_Node* newNode = wait_free_create_node(data);
    WAIT_FREE_Node* oldHead = atomic_load(head);
    do {
        atomic_store(&newNode->next, oldHead);
    } while (!atomic_compare_exchange_weak(head, &oldHead, newNode));
}

void wait_free_list_delete(_Atomic(WAIT_FREE_Node*)* head, int data) {
    WAIT_FREE_Node* pred = NULL;
    WAIT_FREE_Node* curr = atomic_load(head);
    WAIT_FREE_Node* succ;

    while (true) {
        if (!curr) return; // If list is empty or end reached without finding the data
        
        succ = atomic_load(&curr->next);
        
        if (curr->data == data) {
            WAIT_FREE_Node* succUnmarked = UNMARKED_PTR(succ);
            // Try to logically delete the node by marking its successor
            if (atomic_compare_exchange_weak(&curr->next, &succ, MARKED_PTR(succUnmarked))) {
                // If successful, attempt to physically remove the node
                if (pred) {
                    if (!atomic_compare_exchange_strong(&pred->next, &curr, succUnmarked)) {
                        // If CAS failed, curr could have been removed or marked by another thread, retry from head
                        curr = atomic_load(head);
                        continue;
                    }
                } else {
                    // Special case when deleting head
                    if (!atomic_compare_exchange_strong(head, &curr, succUnmarked)) {
                        // If CAS failed, head has changed, retry
                        curr = atomic_load(head);
                        continue;
                    }
                }
                free(curr);// Free outside the critical section; consider safe memory reclamation techniques
                return;
            }
        } else {
            pred = curr;
            curr = UNMARKED_PTR(succ);
        }
    }
}

int wait_free_list_search(_Atomic(WAIT_FREE_Node*)* head, int data) {
    WAIT_FREE_Node* curr = atomic_load(head);
    while (curr != NULL) {
        if (curr->data == data) {
            return 1;// Found
        }
        curr = atomic_load(&curr->next);
    }
    return 0;// Not found
}

// Wait-Free Guarantees: The provided code aims to minimize blocking but does not fully achieve wait-freedom. True wait-freedom in deletion operations often requires additional mechanisms to ensure all threads can make progress without interference.

