
// Naive Locking linked list
// #include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "nl_linkedlist.h"

// typedef struct Node {
//     int data;
//     struct Node* next;
// } Node;

// typedef struct {
//     Node* head;
//     pthread_mutex_t lock;
// } LinkedList;

void nl_list_init(NL_LinkedList* list) {
    list->head = NULL;
    pthread_mutex_init(&list->lock, NULL);
}

void nl_list_insert(NL_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock);
    NL_Node* newNode = (NL_Node*)malloc(sizeof(NL_Node));
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
    pthread_mutex_unlock(&list->lock);
}

void nl_list_delete(NL_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock);
    
    NL_Node* temp = list->head;
    NL_Node* prev = NULL;
    
    // If the head node itself holds the key to be deleted
    if (temp != NULL && temp->data == data) {
        list->head = temp->next; // Change head
        free(temp);              // free old head
        pthread_mutex_unlock(&list->lock);
        return;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL) {
        pthread_mutex_unlock(&list->lock);
        return;
    }

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp);  // Free memory
    pthread_mutex_unlock(&list->lock);
}

int nl_list_search(NL_LinkedList* list, int data) {
    pthread_mutex_lock(&list->lock);
    
    NL_Node* current = list->head;  // Initialize current
    while (current != NULL) {
        if (current->data == data) {
            pthread_mutex_unlock(&list->lock);
            return 1; // Data found
        }
        current = current->next;
    }
    pthread_mutex_unlock(&list->lock);
    return 0; // Data not found
}



