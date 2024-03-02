
//Fine-Grained Locking linked list with RW Lock
#include <stdlib.h>
#include <stdio.h>
#include "fgl_linkedlist.h"

void fgl_list_init(FGL_Node** head) {
    // Allocate and initialize your head node here, if necessary
    *head = NULL;
}


FGL_Node* fgl_create_node(int data) {
    FGL_Node* newNode = (FGL_Node*)malloc(sizeof(FGL_Node));
    if (newNode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    pthread_rwlock_init(&newNode->lock, NULL); // Initialize reader-writer lock
    return newNode;
}


void fgl_list_insert(FGL_Node** head, int data) {
    FGL_Node* newNode = fgl_create_node(data);

    if (*head != NULL) {
        pthread_rwlock_wrlock(&(*head)->lock); // Acquire a write lock on the head
    }

    newNode->next = *head; // Insert the new node at the beginning
    *head = newNode; // Update the head pointer to the new node

    if (newNode->next != NULL) {
        pthread_rwlock_unlock(&newNode->next->lock); // Unlock the previous head
    }
    // No need to lock the new head at the end of this function
}

void fgl_list_delete(FGL_Node **head, int data) {
    if (*head == NULL) return; // Early return if the list is empty

    FGL_Node *temp = *head, *prev = NULL;

    pthread_rwlock_wrlock(&(*head)->lock); // Acquire a write lock on the head node before any operations

    if (temp->data == data) {
        *head = temp->next; // Change head
        pthread_rwlock_unlock(&temp->lock); // Unlock before freeing
        pthread_rwlock_destroy(&temp->lock); // Destroy the lock
        free(temp); // Free old head
        if (*head != NULL) {
            pthread_rwlock_unlock(&(*head)->lock); // Unlock the new head, if it exists
        }
        return;
    }

    while (temp != NULL && temp->data != data) {
        if (prev != NULL) {
            pthread_rwlock_unlock(&prev->lock); // Unlock the previous node after moving forward
        }
        prev = temp;
        temp = temp->next;
        if (temp != NULL) {
            pthread_rwlock_wrlock(&temp->lock); // Acquire a write lock on the next node
        }
    }

    if (temp == NULL) { // If key was not present in linked list
        if (prev != NULL) {
            pthread_rwlock_unlock(&prev->lock); // Ensure the previous node is unlocked
        }
        return;
    }

    if (prev != NULL) {
        prev->next = temp->next; // Unlink the node from linked list
    }

    pthread_rwlock_unlock(&temp->lock); // Unlock before freeing
    pthread_rwlock_destroy(&temp->lock); // Destroy the lock
    free(temp); // Free the memory

    if (prev != NULL) {
        pthread_rwlock_unlock(&prev->lock); // Unlock the previous node as the operation is complete
    }
}

int fgl_list_search(FGL_Node* head, int data) {
    FGL_Node* current = head;
    while (current != NULL) {
        pthread_rwlock_rdlock(&current->lock); // Acquire read lock for current node
        if (current->data == data) {
            pthread_rwlock_unlock(&current->lock); // Release read lock before returning
            return 1; // Data found
        }
        pthread_rwlock_unlock(&current->lock); // Release read lock
        current = current->next; // Move to the next node
    }
    return 0; // Data not found
}


// Note: Remember to compile with the -pthread flag.
// gcc -pg fgl_linkedlist.c -o fgl_linkedlist -pthread -O3