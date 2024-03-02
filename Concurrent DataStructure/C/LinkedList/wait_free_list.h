// wait_free_list.h
#ifndef WAIT_FREE_LIST_H
#define WAIT_FREE_LIST_H

#include <stdatomic.h>
#include <pthread.h>

#define MARKED_PTR(p)   (WAIT_FREE_Node*)((uintptr_t)(p) | 1)
#define UNMARKED_PTR(p) (WAIT_FREE_Node*)((uintptr_t)(p) & ~1)
#define IS_MARKED(p)    ((uintptr_t)(p) & 1)

typedef struct WAIT_FREE_Node {
    int data;
    _Atomic(struct WAIT_FREE_Node*) next;
} WAIT_FREE_Node;

typedef struct WAIT_FREE_LockFreeList {
    _Atomic(WAIT_FREE_Node*) head;
} WAIT_FREE_LockFreeList;

void wait_free_list_init(WAIT_FREE_LockFreeList* list);
WAIT_FREE_Node* create_node(int data);
void wait_free_list_insert(_Atomic(WAIT_FREE_Node*)* head, int data);
void wait_free_list_delete(_Atomic(WAIT_FREE_Node*)* head, int data);
int wait_free_list_search(_Atomic(WAIT_FREE_Node*)* head, int data);

#endif // WAIT_FREE_LIST_H
