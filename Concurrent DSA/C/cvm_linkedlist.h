// cvm_linkedlist.h
#ifndef CVM_LINKEDLIST_H
#define CVM_LINKEDLIST_H

#include <pthread.h>

typedef struct CVM_Node {
    int data;
    struct CVM_Node* next;
} CVM_Node;

typedef struct {
    CVM_Node* head;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} CVM_LinkedList;

void cvm_list_init(CVM_LinkedList* list);
CVM_Node* cvm_create_node(int data);
void cvm_list_insert(CVM_LinkedList* list, int data);
void cvm_list_delete(CVM_LinkedList* list, int data);
int cvm_list_search(CVM_LinkedList* list, int data);

#endif // CVM_LINKEDLIST_H
