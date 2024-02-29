// lock_free_list.h
#ifndef LOCK_FREE_LIST_H
#define LOCK_FREE_LIST_H

#include <stdatomic.h>

typedef struct LOCK_FREE_Node {
    int data;
    atomic_uintptr_t next; // Points to the next Node
} LOCK_FREE_Node;

typedef struct LockFreeList {
    atomic_uintptr_t head;
} LockFreeList;

// Function declarations
void Lock_Free_list_init(LockFreeList* list);
LOCK_FREE_Node* Lock_Free_create_node(int data);
void Lock_Free_list_insert(LockFreeList* list, int data);
int Lock_Free_list_delete(LockFreeList* list, int data);
int Lock_Free_list_search(LockFreeList* list, int data);

#endif // LOCK_FREE_LIST_H
