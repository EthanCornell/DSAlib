// fgl_linkedlist.h
#ifndef FGL_LINKEDLIST_H
#define FGL_LINKEDLIST_H


#include <pthread.h>

// Node structure with a fine-grained lock
// typedef struct FGL_Node {
//     int data;
//     struct FGL_Node* next;
//     pthread_mutex_t lock;
// } FGL_Node;

typedef struct FGL_Node {
    int data;
    struct FGL_Node* next;
    pthread_rwlock_t lock; // Change to reader-writer lock
} FGL_Node;

// Function prototypes
void fgl_list_init(FGL_Node** head);
FGL_Node* fgl_create_node(int data);
void fgl_list_insert(FGL_Node** head, int data);
void fgl_list_delete(FGL_Node** head, int data);
int fgl_list_search(FGL_Node* head, int data);

#endif // FGL_LINKEDLIST_H

