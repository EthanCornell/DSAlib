
// Condition Variables with Mutexes linked list 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "cvm_linkedlist.h"

// typedef struct Node {
//     int data;
//     struct Node* next;
// } Node;

// typedef struct {
//     Node* head;
//     pthread_mutex_t lock;
//     pthread_cond_t cond;
// } LinkedList;

void cvm_list_init(CVM_LinkedList* list) {
    list->head = NULL;
    pthread_mutex_init(&list->lock, NULL);
    pthread_cond_init(&list->cond, NULL);
}

CVM_Node* cvm_create_node(int data) {
    CVM_Node* newNode = (CVM_Node*)malloc(sizeof(CVM_Node));
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// For deletion, you might wait on a condition variable if the list is empty
void cvm_list_delete(CVM_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock);
    
    // Wait while the list is empty
    while (list->head == NULL) {
        pthread_cond_wait(&list->cond, &list->lock);
    }
    
    CVM_Node* current = list->head;
    CVM_Node* prev = NULL;
    
    // Search for the node to delete
    while (current != NULL) {
        if (current->data == data) {
            // Node found, perform deletion
            if (prev == NULL) {
                // The node to delete is the head
                list->head = current->next;
            } else {
                // The node to delete is not the head
                prev->next = current->next;
            }
            free(current);
            pthread_mutex_unlock(&list->lock);
            return;
        }
        prev = current;
        current = current->next;
    }

    pthread_mutex_unlock(&list->lock);
    // If the function reaches this point, the data was not found,
    // or the list was empty, in which case we've already returned.
}


// When inserting, you might signal a waiting thread
void cvm_list_insert(CVM_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock);
    
    // Create and initialize a new node
    CVM_Node* newNode = cvm_create_node(data);

    // Insert the new node at the beginning of the list
    newNode->next = list->head;
    list->head = newNode;

    // Signal that the list is not empty (if it was before)
    pthread_cond_signal(&list->cond);
    pthread_mutex_unlock(&list->lock);
}



int cvm_list_search(CVM_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock); // Lock the mutex before accessing the list

    CVM_Node* current = list->head;
    while (current != NULL) {
        if (current->data == data) {
            pthread_mutex_unlock(&list->lock); // Unlock the mutex before returning
            return 1; // Data found
        }
        current = current->next;
    }

    pthread_mutex_unlock(&list->lock); // Make sure to unlock the mutex if data is not found
    return 0; // Data not found
}

// Compile with: gcc -pg cvm_linkedlist.c -o cvm_linkedlist -pthread -O3