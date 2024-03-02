#ifndef NLLINKEDLIST_H
#define NLLINKEDLIST_H

#include <pthread.h>

// Node structure
typedef struct NL_Node {
    int data;
    struct NL_Node* next;
} NL_Node;

// LinkedList structure
typedef struct {
    NL_Node* head;
    pthread_mutex_t lock;
} NL_LinkedList;

// Function declarations
void nl_list_init(NL_LinkedList* list);
void nl_list_insert(NL_LinkedList* list, int data);
void nl_list_delete(NL_LinkedList* list, int data);
int nl_list_search(NL_LinkedList* list, int data);

#endif // LINKEDLIST_H
