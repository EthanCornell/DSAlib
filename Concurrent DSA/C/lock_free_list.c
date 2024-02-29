
// lock-free linked list
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> // Add this line
#include "lock_free_list.h"
#include <pthread.h>

// typedef struct Node {
//     int data;
//     atomic_uintptr_t next; // Points to the next Node
// } Node;

// typedef struct LockFreeList {
//     atomic_uintptr_t head;
// } LockFreeList;

void Lock_Free_list_init(LockFreeList* list) {
    atomic_store(&list->head, (uintptr_t)NULL);
}

LOCK_FREE_Node* Lock_Free_create_node(int data) {
    LOCK_FREE_Node* node = (LOCK_FREE_Node*)malloc(sizeof(LOCK_FREE_Node));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    atomic_store(&node->next, (uintptr_t)NULL);
    return node;
}

void Lock_Free_list_insert(LockFreeList* list, int data) {
    LOCK_FREE_Node* newNode = Lock_Free_create_node(data);
    LOCK_FREE_Node* oldHead;
    do {
        oldHead = (LOCK_FREE_Node*)atomic_load(&list->head);
        atomic_store(&newNode->next, (uintptr_t)oldHead);
    } while (!atomic_compare_exchange_weak(&list->head, (uintptr_t*)&oldHead, (uintptr_t)newNode));
}


int Lock_Free_list_delete(LockFreeList* list, int data) {
    LOCK_FREE_Node* prev = NULL;
    LOCK_FREE_Node* curr = (LOCK_FREE_Node*)atomic_load(&list->head);
    LOCK_FREE_Node* next = NULL;

    while (curr != NULL) {
        next = (LOCK_FREE_Node*)atomic_load(&(curr->next));

        if (curr->data == data) { // Found node to delete
            if (prev == NULL) { // Head needs to be deleted
                if (atomic_compare_exchange_weak(&list->head, (uintptr_t*)&curr, (uintptr_t)next)) {
                    free(curr);
                    return 1; // Success
                }
            } else if (atomic_compare_exchange_weak(&(prev->next), (uintptr_t*)&curr, (uintptr_t)next)) {
                free(curr);
                return 1; // Success
            }
            // If CAS failed, start over
            curr = (LOCK_FREE_Node*)atomic_load(&list->head);
        } else {
            prev = curr;
            curr = next;
        }
    }

    return 0; // Not found or failed
}

int Lock_Free_list_search(LockFreeList* list, int data) {
    LOCK_FREE_Node* curr = (LOCK_FREE_Node*)atomic_load(&list->head);

    while (curr != NULL) {
        if (curr->data == data) {
            return 1; // Found
        }
        curr = (LOCK_FREE_Node*)atomic_load(&(curr->next));
    }

    return 0; // Not found
}



